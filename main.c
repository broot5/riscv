#include <stdbool.h>

#include "cpu.h"
#include "instruction.h"
#include "utils.h"

void load_program(CPU_t *cpu, const char *filename) {
  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("Failed to open program file");
    cpu->halt = true;
    return;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  rewind(fp);

  if (file_size == -1L) {
    perror("Failed to get program file size");
    cpu->halt = true;
    fclose(fp);
    return;
  }

  size_t load_offset_bytes = PROGRAM_LOAD_VADDR - MEMORY_BASE_ADDR;

  if (load_offset_bytes >= cpu->mem_size) {
    fprintf(
        stderr,
        "Program load virtual address (0x%08x) is beyond emulated memory bounds"
        "(base: 0x%08x, size: %zu bytes)\n",
        PROGRAM_LOAD_VADDR, MEMORY_BASE_ADDR, cpu->mem_size);
    cpu->halt = true;
    fclose(fp);
    return;
  }

  if ((size_t)file_size > (cpu->mem_size - load_offset_bytes)) {
    fprintf(stderr,
            "Program size (%ld bytes) exceeds available memory at load address"
            "(0x%08x, available: %zu bytes)\n",
            file_size, PROGRAM_LOAD_VADDR, (cpu->mem_size - load_offset_bytes));
    cpu->halt = true;
    fclose(fp);
    return;
  }

  size_t bytes_read_count =
      fread(&cpu->memory[load_offset_bytes], 1, file_size, fp);

  if (ferror(fp)) {
    perror("Error reading program file during load");
    cpu->halt = true;
    fclose(fp);
    return;
  }

  if (bytes_read_count != (size_t)file_size) {
    fprintf(stderr, "Only %zu of %ld bytes read from program file %s\n",
            bytes_read_count, file_size, filename);
  }

  fclose(fp);

  printf("Loaded %zu bytes from %s into emulated memory\n", bytes_read_count,
         filename);
  return;
}

int main() {
  CPU_t cpu;
  init_cpu(&cpu);
  init_dispatch_table();

  if (cpu.halt) {
    free_cpu(&cpu);
    return EXIT_FAILURE;
  }

  load_program(&cpu, "test/test.bin");

  if (cpu.halt) {
    free_cpu(&cpu);
    return EXIT_FAILURE;
  }

  while (!cpu.halt) {
    uint32_t instruction = fetch_instruction(&cpu);

    dispatch_table[get_opcode(instruction)][get_funct3(instruction)](
        instruction, &cpu);

    printf("instruction: 0x%08x\n", instruction);
    printf("pc: 0x%08x\n", cpu.pc);
    printf("regs:");
    for (int i = 0; i < 32; i++) {
      printf(" %d", read_reg(&cpu, i));
    }
    printf("\n\n");
  }

  free_cpu(&cpu);

  return cpu.halt ? EXIT_FAILURE : EXIT_SUCCESS;
}