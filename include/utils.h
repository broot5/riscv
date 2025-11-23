#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

static inline uint8_t get_opcode(uint32_t inst) {
  return (uint8_t)(inst & 0x7F);
}

static inline uint8_t get_funct3(uint32_t inst) {
  return (uint8_t)((inst >> 12) & 0x7);
}

static inline uint8_t get_funct7(uint32_t inst) {
  return (uint8_t)((inst >> 25) & 0x7F);
}

static inline uint8_t get_rd(uint32_t inst) {
  return (uint8_t)((inst >> 7) & 0x1F);
}

static inline uint8_t get_rs1(uint32_t inst) {
  return (uint8_t)((inst >> 15) & 0x1F);
}

static inline uint8_t get_rs2(uint32_t inst) {
  return (uint8_t)((inst >> 20) & 0x1F);
}

static inline int32_t sign_extend(uint32_t value, int num_bits) {
  return (int32_t)(value << (32 - num_bits)) >> (32 - num_bits);
}

static inline int32_t get_imm_i(uint32_t inst) {
  uint32_t imm_raw = (inst >> 20) & 0xFFF;
  return sign_extend(imm_raw, 12);
}

static inline int32_t get_imm_s(uint32_t inst) {
  uint32_t imm_11_5 = (inst >> 25) & 0x7F;
  uint32_t imm_4_0 = (inst >> 7) & 0x1F;
  uint32_t imm_raw = (imm_11_5 << 5) | imm_4_0;
  return sign_extend(imm_raw, 12);
}

static inline int32_t get_imm_b(uint32_t inst) {
  uint32_t imm_12 = (inst >> 31) & 0x1;
  uint32_t imm_10_5 = (inst >> 25) & 0x3F;
  uint32_t imm_4_1 = (inst >> 8) & 0xF;
  uint32_t imm_11 = (inst >> 7) & 0x1;
  uint32_t imm_raw =
      (imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | (imm_4_1 << 1);
  return sign_extend(imm_raw, 13);
}

static inline uint32_t get_imm_u(uint32_t inst) { return inst & 0xFFFFF000; }

static inline int32_t get_imm_j(uint32_t inst) {
  uint32_t imm_20 = (inst >> 31) & 0x1;
  uint32_t imm_10_1 = (inst >> 21) & 0x3FF;
  uint32_t imm_11 = (inst >> 20) & 0x1;
  uint32_t imm_19_12 = (inst >> 12) & 0xFF;
  uint32_t imm_raw =
      (imm_20 << 20) | (imm_19_12 << 12) | (imm_11 << 11) | (imm_10_1 << 1);
  return sign_extend(imm_raw, 21);
}

#endif