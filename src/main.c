#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "cpu.h"
#include "decoder.h"
#include "emulator.h"
#include "loader.h"

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
    RvStepResult result = rv_step(&cpu, dispatch_table);

    if (result.status != RV_STEP_EXECUTED) {
      break;
    }
  }

  if (cpu.exit_code != 0) {
    dump_registers(&cpu);
  }

  free_cpu(&cpu);

  return cpu.exit_code;
}
