#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "cpu.h"
#include "decoder.h"
#include "loader.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <program.elf>\n", argv[0]);
    return EXIT_FAILURE;
  }

  CPU_t cpu;
  init_cpu(&cpu);

  InstructionHandler dispatch_table[128][8];
  init_dispatch_table(dispatch_table);

  load_elf(&cpu, argv[1]);

  if (cpu.halt) {
    free_cpu(&cpu);
    return EXIT_FAILURE;
  }

  while (!cpu.halt) {
    uint32_t instruction = fetch_instruction(&cpu);
    int step = 4;

    cpu.next_pc = cpu.pc + step;

    dispatch_table[get_opcode(instruction)][get_funct3(instruction)](
        instruction, &cpu);

    if (cpu.halt)
      break;

    cpu.pc = cpu.next_pc;
  }

  if (cpu.exit_code != 0) {
    dump_registers(&cpu);
  }

  free_cpu(&cpu);

  return cpu.exit_code;
}