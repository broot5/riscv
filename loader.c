#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "loader.h"

void load_elf(CPU_t *cpu, const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("Error: Failed to open program file");
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  Elf32_Ehdr_t elf_header;
  if (fread(&elf_header, sizeof(Elf32_Ehdr_t), 1, fp) != 1) {
    fprintf(stderr, "Error: Failed to read ELF header\n");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Validate ELF Magic Number
  if (elf_header.e_ident[0] != 0x7F || elf_header.e_ident[1] != 'E' ||
      elf_header.e_ident[2] != 'L' || elf_header.e_ident[3] != 'F') {
    fprintf(stderr, "Error: Not a valid ELF file\n");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Validate Class (1 = 32-bit)
  if (elf_header.e_ident[4] != 1) {
    fprintf(stderr, "Error: Not a 32-bit ELF file\n");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Validate Executable File
  if (elf_header.e_type != 2) {
    fprintf(stderr, "Error: Not an executable ELF file (e_type=%d)\n",
            elf_header.e_type);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Validate Machine Architecture (RISC-V)
  if (elf_header.e_machine != 243) {
    fprintf(stderr, "Error: Not a RISC-V ELF file (e_machine=%d)\n",
            elf_header.e_machine);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  if (elf_header.e_version != 1) {
    fprintf(stderr, "Error: Invalid ELF version (e_version=%d)\n",
            elf_header.e_version);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  Elf32_Phdr_t *program_headers =
      (Elf32_Phdr_t *)malloc(sizeof(Elf32_Phdr_t) * elf_header.e_phnum);
  if (!program_headers) {
    perror("Error: Failed to allocate memory for program headers");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Read all program headers
  for (int i = 0; i < elf_header.e_phnum; i++) {
    if (fseek(fp, elf_header.e_phoff + i * elf_header.e_phentsize, SEEK_SET) !=
        0) {
      perror("Error: Failed to seek to program header");
      free(program_headers);
      fclose(fp);
      cpu->exit_code = 1;
      cpu->halt = true;
      return;
    }

    if (fread(&program_headers[i], sizeof(Elf32_Phdr_t), 1, fp) != 1) {
      fprintf(stderr, "Error: Failed to read program header %d\n", i);
      free(program_headers);
      fclose(fp);
      cpu->exit_code = 1;
      cpu->halt = true;
      return;
    }
  }

  // Find the lowest virtual address (Base Address)
  uint32_t min_vaddr = 0xFFFFFFFF;
  bool found_loadable = false;

  for (int i = 0; i < elf_header.e_phnum; i++) {
    if (program_headers[i].p_type == 1) { // PT_LOAD
      if (program_headers[i].p_vaddr < min_vaddr) {
        min_vaddr = program_headers[i].p_vaddr;
        found_loadable = true;
      }
    }
  }

  if (found_loadable) {
    cpu->memory_base = min_vaddr;
    // Update Stack Pointer (x2) to be at the top of the new memory range
    cpu->regs[2] = cpu->memory_base + cpu->mem_size;
  }

  // Load segments
  for (int i = 0; i < elf_header.e_phnum; i++) {
    Elf32_Phdr_t *ph = &program_headers[i];

    // Handle PT_LOAD Segment
    if (ph->p_type == 1) {
      if (ph->p_vaddr < cpu->memory_base ||
          ph->p_vaddr - cpu->memory_base + ph->p_memsz > cpu->mem_size) {
        fprintf(stderr,
                "Error: Segment load address out of bounds (0x%08x + %u, base: "
                "0x%08x)\n",
                ph->p_vaddr, ph->p_memsz, cpu->memory_base);
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      uint8_t *dest = &cpu->memory[ph->p_vaddr - cpu->memory_base];

      if (fseek(fp, ph->p_offset, SEEK_SET) != 0) {
        perror("Error: Failed to seek to segment data");
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      if (ph->p_filesz > 0) {
        if (fread(dest, 1, ph->p_filesz, fp) != ph->p_filesz) {
          fprintf(stderr, "Error: Failed to read segment data for segment %d\n",
                  i);
          free(program_headers);
          fclose(fp);
          cpu->exit_code = 1;
          cpu->halt = true;
          return;
        }
      }

      if (ph->p_memsz > ph->p_filesz) {
        size_t bss_size = ph->p_memsz - ph->p_filesz;
        memset(dest + ph->p_filesz, 0, bss_size);
      }
    }
  }

  free(program_headers);
  fclose(fp);

  cpu->pc = elf_header.e_entry;
}
