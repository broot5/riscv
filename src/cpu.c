#include "cpu.h"

#include <stdio.h>
#include <string.h>

void init_cpu(CPU_t *cpu) {
  memset(cpu->regs, 0, sizeof(cpu->regs));
  cpu->pc = 0;
  cpu->next_pc = 0;
  cpu->current_inst_len = 0;

  cpu->exit_code = 0;
  cpu->halt = false;
}

uint32_t read_reg(CPU_t *cpu, unsigned int idx) {
  if (idx >= 32) {
    fprintf(stderr, "Error: Invalid register read access: x%u\n", idx);
    cpu->exit_code = 1;
    cpu->halt = true;
    return 0;
  }
  if (idx == 0)
    return 0;
  return cpu->regs[idx];
}

void write_reg(CPU_t *cpu, unsigned int idx, uint32_t value) {
  if (idx >= 32) {
    fprintf(stderr, "Error: Invalid register write access: x%u\n", idx);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }
  if (idx == 0)
    return;
  cpu->regs[idx] = value;
}

void dump_registers(CPU_t *cpu) {
  fprintf(stderr,
          "================================================================\n");
  fprintf(stderr, "Register Dump:\n");
  fprintf(stderr, "PC : 0x%08x\n", cpu->pc);
  for (int i = 0; i < 32; i++) {
    fprintf(stderr, "x%-2d: 0x%08x ", i, cpu->regs[i]);
    if ((i + 1) % 4 == 0)
      fprintf(stderr, "\n");
  }
  fprintf(stderr,
          "================================================================\n");
}
