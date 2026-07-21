#include <limits.h>
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

  // Validate Class (32-bit)
  if (elf_header.e_ident[EI_CLASS] != ELFCLASS32) {
    fprintf(stderr, "Error: Not a 32-bit ELF file\n");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Validate Executable File
  if (elf_header.e_type != ET_EXEC) {
    fprintf(stderr, "Error: Not an executable ELF file (e_type=%d)\n",
            elf_header.e_type);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  // Validate Machine Architecture (RISC-V)
  if (elf_header.e_machine != EM_RISCV) {
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

  if (fseek(fp, 0, SEEK_END) != 0) {
    perror("Error: Failed to seek to end of ELF file");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  long file_size_result = ftell(fp);
  if (file_size_result < 0) {
    perror("Error: Failed to determine ELF file size");
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }
  uint64_t file_size = (uint64_t)file_size_result;

  if (elf_header.e_phentsize != sizeof(Elf32_Phdr_t)) {
    fprintf(stderr, "Error: Invalid program header size (e_phentsize=%u)\n",
            elf_header.e_phentsize);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  uint64_t program_header_size =
      (uint64_t)elf_header.e_phnum * elf_header.e_phentsize;
  if ((uint64_t)elf_header.e_phoff > file_size ||
      program_header_size > file_size - elf_header.e_phoff) {
    fprintf(stderr, "Error: Program header table extends beyond ELF file\n");
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
    uint64_t program_header_offset =
        (uint64_t)elf_header.e_phoff +
        (uint64_t)i * elf_header.e_phentsize;
    if (program_header_offset > LONG_MAX ||
        fseek(fp, (long)program_header_offset, SEEK_SET) != 0) {
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

  uint32_t min_vaddr = 0xFFFFFFFF; // lowest virtual address (Base Address)
  uint32_t max_vaddr = 0;          // highest virtual address (Program Break)
  bool found_loadable = false;

  for (int i = 0; i < elf_header.e_phnum; i++) {
    if (program_headers[i].p_type == PT_LOAD) {
      Elf32_Phdr_t *ph = &program_headers[i];
      found_loadable = true;

      if (ph->p_filesz > ph->p_memsz) {
        fprintf(stderr,
                "Error: Segment file size exceeds memory size "
                "(filesz=%u, memsz=%u)\n",
                ph->p_filesz, ph->p_memsz);
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      if (ph->p_memsz > UINT32_MAX - ph->p_vaddr) {
        fprintf(stderr,
                "Error: Segment virtual address range overflows 32 bits "
                "(0x%08x + %u)\n",
                ph->p_vaddr, ph->p_memsz);
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      if ((uint64_t)ph->p_offset > file_size ||
          (uint64_t)ph->p_filesz > file_size - ph->p_offset) {
        fprintf(stderr,
                "Error: Segment data extends beyond ELF file "
                "(offset=%u, filesz=%u)\n",
                ph->p_offset, ph->p_filesz);
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      if (ph->p_vaddr < min_vaddr) {
        min_vaddr = ph->p_vaddr;
      }

      uint32_t end_vaddr = ph->p_vaddr + ph->p_memsz;
      if (end_vaddr > max_vaddr) {
        max_vaddr = end_vaddr;
      }
    }
  }

  if (!found_loadable) {
    fprintf(stderr, "Error: No loadable segments found in ELF file\n");
    free(program_headers);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  if (cpu->mem_size > UINT32_MAX ||
      min_vaddr > UINT32_MAX - (uint32_t)cpu->mem_size) {
    fprintf(stderr,
            "Error: Guest memory range overflows 32-bit address space "
            "(base=0x%08x, size=%zu)\n",
            min_vaddr, cpu->mem_size);
    free(program_headers);
    fclose(fp);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  cpu->memory_base = min_vaddr;
  cpu->program_break = max_vaddr;
  // Update Stack Pointer (x2) to be at the top of the new memory range
  cpu->regs[2] = cpu->memory_base + cpu->mem_size;

  // Load segments
  for (int i = 0; i < elf_header.e_phnum; i++) {
    Elf32_Phdr_t *ph = &program_headers[i];

    // Handle PT_LOAD Segment
    if (ph->p_type == PT_LOAD) {
      if (ph->p_vaddr < cpu->memory_base) {
        fprintf(stderr,
                "Error: Segment load address below memory base "
                "(address=0x%08x, base=0x%08x)\n",
                ph->p_vaddr, cpu->memory_base);
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      size_t offset = (size_t)(ph->p_vaddr - cpu->memory_base);
      if (offset > cpu->mem_size ||
          ph->p_memsz > cpu->mem_size - offset ||
          ph->p_filesz > cpu->mem_size - offset) {
        fprintf(stderr,
                "Error: Segment load range out of bounds "
                "(address=0x%08x, filesz=%u, memsz=%u, base=0x%08x)\n",
                ph->p_vaddr, ph->p_filesz, ph->p_memsz, cpu->memory_base);
        free(program_headers);
        fclose(fp);
        cpu->exit_code = 1;
        cpu->halt = true;
        return;
      }

      uint8_t *dest = &cpu->memory[offset];

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
