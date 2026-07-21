#include "cpu.h"
#include "emulator.h"
#include "loader.h"
#include "memory.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <program.elf>\n", argv[0]);
    return EXIT_FAILURE;
  }

  CPU_t cpu;
  init_cpu(&cpu);

  Memory_t memory;
  if (!init_memory(&memory, MEMORY_SIZE_BYTES))
    return EXIT_FAILURE;

  RvContext_t context = {.cpu = &cpu, .memory = &memory};

  load_elf(&cpu, &memory, argv[1]);

  if (cpu.halt) {
    free_memory(&memory);
    return EXIT_FAILURE;
  }

  while (!cpu.halt) {
    RvStepResult result = rv_step(&context);

    if (result.status != RV_STEP_EXECUTED) {
      break;
    }
  }

  if (cpu.exit_code != 0) {
    dump_registers(&cpu);
  }

  free_memory(&memory);

  return cpu.exit_code;
}
