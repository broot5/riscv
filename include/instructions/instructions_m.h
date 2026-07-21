#ifndef INSTRUCTIONS_M_H
#define INSTRUCTIONS_M_H

#include <stdint.h>

#include "cpu.h"

void handle_mul(uint32_t inst, CPU_t *cpu);
void handle_mulh(uint32_t inst, CPU_t *cpu);
void handle_mulhsu(uint32_t inst, CPU_t *cpu);
void handle_mulhu(uint32_t inst, CPU_t *cpu);
void handle_div(uint32_t inst, CPU_t *cpu);
void handle_divu(uint32_t inst, CPU_t *cpu);
void handle_rem(uint32_t inst, CPU_t *cpu);
void handle_remu(uint32_t inst, CPU_t *cpu);

#endif
