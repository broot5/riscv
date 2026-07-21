#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>

typedef struct CPU {
  uint32_t regs[32];
  uint32_t pc;
  uint32_t next_pc;
  uint32_t current_inst_len;
  int exit_code;
  bool halt;
} CPU_t;

void init_cpu(CPU_t *cpu);

uint32_t read_reg(CPU_t *cpu, unsigned int idx);
void write_reg(CPU_t *cpu, unsigned int idx, uint32_t value);
void dump_registers(CPU_t *cpu);

#endif
