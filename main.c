#include <stdbool.h>
#include <unistd.h>

#include "cpu.h"
#include "instruction.h"
#include "utils.h"

void load_program(CPU_t *cpu, const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("Error: Failed to open program file");
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);

  if (file_size == -1L) {
    perror("Error: Failed to get program file size");
    cpu->exit_code = 1;
    cpu->halt = true;
    fclose(fp);
    return;
  }

  size_t load_offset_bytes = PROGRAM_LOAD_VADDR - MEMORY_BASE_ADDR;

  if (load_offset_bytes >= cpu->mem_size) {
    fprintf(stderr,
            "Error: Program load virtual address (0x%08x) is beyond emulated "
            "memory bounds"
            "(base: 0x%08x, size: %zu bytes)\n",
            PROGRAM_LOAD_VADDR, MEMORY_BASE_ADDR, cpu->mem_size);
    cpu->exit_code = 1;
    cpu->halt = true;
    fclose(fp);
    return;
  }

  if ((size_t)file_size > (cpu->mem_size - load_offset_bytes)) {
    fprintf(stderr,
            "Error: Program size (%ld bytes) exceeds available memory at load "
            "address"
            "(0x%08x, available: %zu bytes)\n",
            file_size, PROGRAM_LOAD_VADDR, (cpu->mem_size - load_offset_bytes));
    cpu->exit_code = 1;
    cpu->halt = true;
    fclose(fp);
    return;
  }

  size_t bytes_read_count =
      fread(&cpu->memory[load_offset_bytes], 1, file_size, fp);

  if (ferror(fp)) {
    perror("Error: Failed to read program file");
    cpu->exit_code = 1;
    cpu->halt = true;
    fclose(fp);
    return;
  }

  if (bytes_read_count != (size_t)file_size) {
    fprintf(stderr,
            "Error: Incomplete read. Only %zu of %ld bytes read from program "
            "file %s\n",
            bytes_read_count, file_size, filename);
  }

  fclose(fp);

  printf("Loaded %zu bytes from %s into emulated memory\n", bytes_read_count,
         filename);
  return;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <program.bin>\n", argv[0]);
    return EXIT_FAILURE;
  }

  CPU_t cpu;
  init_cpu(&cpu);

  InstructionHandler dispatch_table[128][8];
  init_dispatch_table(dispatch_table);

  load_program(&cpu, argv[1]);

  if (cpu.halt) {
    free_cpu(&cpu);
    return EXIT_FAILURE;
  }

  while (!cpu.halt) {
    uint32_t instruction = fetch_instruction(&cpu);

    dispatch_table[get_opcode(instruction)][get_funct3(instruction)](
        instruction, &cpu);
  }

  free_cpu(&cpu);

  return cpu.exit_code;
}