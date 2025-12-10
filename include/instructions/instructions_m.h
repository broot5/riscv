#ifndef INSTRUCTIONS_M_H
#define INSTRUCTIONS_M_H

#include <limits.h>
#include <stdint.h>

#include "cpu.h"
#include "utils.h"

static inline void handle_mul(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val * rs2_val);
}

static inline void handle_mulh(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int64_t result = (int64_t)(int32_t)rs1_val * (int64_t)(int32_t)rs2_val;

  write_reg(cpu, get_rd(inst), (uint32_t)(result >> 32));
}

static inline void handle_mulhsu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int64_t result = (int64_t)(int32_t)rs1_val * (uint64_t)rs2_val;

  write_reg(cpu, get_rd(inst), (uint32_t)(result >> 32));
}

static inline void handle_mulhu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  uint64_t result = (uint64_t)rs1_val * (uint64_t)rs2_val;

  write_reg(cpu, get_rd(inst), (uint32_t)(result >> 32));
}

static inline void handle_div(uint32_t inst, CPU_t *cpu) {
  int32_t rs1_val = (int32_t)read_reg(cpu, get_rs1(inst));
  int32_t rs2_val = (int32_t)read_reg(cpu, get_rs2(inst));

  if (rs2_val == 0) {
    write_reg(cpu, get_rd(inst), -1);
  } else if (rs1_val == INT32_MIN && rs2_val == -1) {
    write_reg(cpu, get_rd(inst), rs1_val);
  } else {
    write_reg(cpu, get_rd(inst), rs1_val / rs2_val);
  }
}

static inline void handle_divu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs2_val == 0) {
    write_reg(cpu, get_rd(inst), UINT32_MAX);
  } else {
    write_reg(cpu, get_rd(inst), rs1_val / rs2_val);
  }
}

static inline void handle_rem(uint32_t inst, CPU_t *cpu) {
  int32_t rs1_val = (int32_t)read_reg(cpu, get_rs1(inst));
  int32_t rs2_val = (int32_t)read_reg(cpu, get_rs2(inst));

  if (rs2_val == 0) {
    write_reg(cpu, get_rd(inst), rs1_val);
  } else if (rs1_val == INT32_MIN && rs2_val == -1) {
    write_reg(cpu, get_rd(inst), 0);
  } else {
    write_reg(cpu, get_rd(inst), rs1_val % rs2_val);
  }
}

static inline void handle_remu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs2_val == 0) {
    write_reg(cpu, get_rd(inst), rs1_val);
  } else {
    write_reg(cpu, get_rd(inst), rs1_val % rs2_val);
  }
}

#endif
