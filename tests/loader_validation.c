#include "cpu.h"
#include "loader.h"
#include "memory.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const uint32_t test_vaddr = 0x00010000;

static Elf32_Ehdr_t make_elf_header(void) {
  Elf32_Ehdr_t header = {0};
  header.e_ident[0] = 0x7F;
  header.e_ident[1] = 'E';
  header.e_ident[2] = 'L';
  header.e_ident[3] = 'F';
  header.e_ident[EI_CLASS] = ELFCLASS32;
  header.e_type = ET_EXEC;
  header.e_machine = EM_RISCV;
  header.e_version = 1;
  header.e_entry = test_vaddr;
  header.e_phoff = sizeof(Elf32_Ehdr_t);
  header.e_phentsize = sizeof(Elf32_Phdr_t);
  header.e_phnum = 1;
  return header;
}

static bool write_test_elf(char path[], const Elf32_Phdr_t *program_header,
                           const uint8_t *data, size_t data_size) {
  int fd = mkstemp(path);
  if (fd < 0) {
    perror("mkstemp");
    return false;
  }

  FILE *fp = fdopen(fd, "wb");
  if (!fp) {
    perror("fdopen");
    close(fd);
    unlink(path);
    return false;
  }

  Elf32_Ehdr_t header = make_elf_header();
  bool ok = fwrite(&header, sizeof(header), 1, fp) == 1 &&
            fwrite(program_header, sizeof(*program_header), 1, fp) == 1;
  if (ok && data_size > 0)
    ok = fwrite(data, 1, data_size, fp) == data_size;

  if (fclose(fp) != 0)
    ok = false;
  if (!ok)
    unlink(path);
  return ok;
}

static bool load_test_elf(const Elf32_Phdr_t *program_header,
                          const uint8_t *data, size_t data_size,
                          bool expect_failure, bool check_contents) {
  char path[] = "/tmp/riscv-loader-test-XXXXXX";
  if (!write_test_elf(path, program_header, data, data_size))
    return false;

  CPU_t cpu;
  init_cpu(&cpu);
  Memory_t memory;
  if (!init_memory(&memory, MEMORY_SIZE_BYTES)) {
    unlink(path);
    return false;
  }
  load_elf(&cpu, &memory, path);

  bool passed = cpu.halt == expect_failure;
  if (passed && check_contents) {
    passed = memory.base == program_header->p_vaddr && cpu.pc == test_vaddr &&
             memory.data[0] == data[0] && memory.data[1] == data[1] &&
             memory.data[2] == data[2] && memory.data[3] == data[3] &&
             memory.data[4] == 0 && memory.data[5] == 0 &&
             memory.data[6] == 0 && memory.data[7] == 0;
  }

  free_memory(&memory);
  unlink(path);
  return passed;
}

static bool test_valid_segment(void) {
  uint8_t data[] = {0x13, 0x00, 0x00, 0x00};
  Elf32_Phdr_t ph = {
      .p_type = PT_LOAD,
      .p_offset = sizeof(Elf32_Ehdr_t) + sizeof(Elf32_Phdr_t),
      .p_vaddr = test_vaddr,
      .p_filesz = sizeof(data),
      .p_memsz = 8,
  };
  return load_test_elf(&ph, data, sizeof(data), false, true);
}

static bool test_filesz_exceeds_memsz(void) {
  uint8_t data[] = {0x13, 0x00};
  Elf32_Phdr_t ph = {
      .p_type = PT_LOAD,
      .p_offset = sizeof(Elf32_Ehdr_t) + sizeof(Elf32_Phdr_t),
      .p_vaddr = test_vaddr,
      .p_filesz = sizeof(data),
      .p_memsz = 1,
  };
  return load_test_elf(&ph, data, sizeof(data), true, false);
}

static bool test_virtual_address_overflow(void) {
  Elf32_Phdr_t ph = {
      .p_type = PT_LOAD,
      .p_offset = sizeof(Elf32_Ehdr_t) + sizeof(Elf32_Phdr_t),
      .p_vaddr = UINT32_MAX - 3,
      .p_filesz = 0,
      .p_memsz = 8,
  };
  return load_test_elf(&ph, NULL, 0, true, false);
}

static bool test_memory_window_overflow(void) {
  Elf32_Phdr_t ph = {
      .p_type = PT_LOAD,
      .p_offset = sizeof(Elf32_Ehdr_t) + sizeof(Elf32_Phdr_t),
      .p_vaddr = UINT32_MAX - (MEMORY_SIZE_BYTES / 2),
      .p_filesz = 0,
      .p_memsz = 1,
  };
  return load_test_elf(&ph, NULL, 0, true, false);
}

static bool test_segment_outside_file(void) {
  Elf32_Phdr_t ph = {
      .p_type = PT_LOAD,
      .p_offset = sizeof(Elf32_Ehdr_t) + sizeof(Elf32_Phdr_t) + 16,
      .p_vaddr = test_vaddr,
      .p_filesz = 1,
      .p_memsz = 1,
  };
  return load_test_elf(&ph, NULL, 0, true, false);
}

int main(void) {
  bool passed = test_valid_segment() && test_filesz_exceeds_memsz() &&
                test_virtual_address_overflow() &&
                test_memory_window_overflow() && test_segment_outside_file();

  if (!passed) {
    fprintf(stderr, "FAIL  loader_validation\n");
    return EXIT_FAILURE;
  }

  printf("PASS  loader_validation\n");
  return EXIT_SUCCESS;
}
