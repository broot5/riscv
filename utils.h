#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

static inline uint32_t get_bits(uint32_t target, int msb, int lsb) {
  uint32_t mask = (1U << (msb - lsb + 1)) - 1;
  return (target >> lsb) & mask;
}

static inline int32_t sign_extend(uint32_t value, int num_bits) {
  if ((value >> (num_bits - 1)) & 0b1) {
    return (int32_t)(value | (~((1U << num_bits) - 1)));
  } else {
    return (int32_t)value;
  }
}

static inline int32_t get_imm_i(uint32_t instruction) {
  uint32_t imm = get_bits(instruction, 31, 20);
  return sign_extend(imm, 12);
}

static inline int32_t get_imm_s(uint32_t instruction) {
  uint32_t imm_11_5 = get_bits(instruction, 31, 25);
  uint32_t imm_4_0 = get_bits(instruction, 11, 7);
  uint32_t imm = (imm_11_5 << 5) | imm_4_0;
  return sign_extend(imm, 12);
}

static inline int32_t get_imm_b(uint32_t instruction) {
  uint32_t imm_12 = get_bits(instruction, 31, 31);
  uint32_t imm_11 = get_bits(instruction, 7, 7);
  uint32_t imm_10_5 = get_bits(instruction, 30, 25);
  uint32_t imm_4_1 = get_bits(instruction, 11, 8);
  uint32_t imm =
      (imm_12 << 12) | (imm_11 << 11) | (imm_10_5 << 5) | (imm_4_1 << 1);
  return sign_extend(imm, 13);
}

static inline int32_t get_imm_u(uint32_t instruction) {
  return get_bits(instruction, 31, 12) << 12;
}

static inline int32_t get_imm_j(uint32_t instruction) {
  uint32_t imm_20 = get_bits(instruction, 31, 31);
  uint32_t imm_19_12 = get_bits(instruction, 19, 12);
  uint32_t imm_11 = get_bits(instruction, 20, 20);
  uint32_t imm_10_1 = get_bits(instruction, 30, 21);
  uint32_t imm =
      (imm_20 << 20) | (imm_19_12 << 12) | (imm_11 << 11) | (imm_10_1 << 1);
  return sign_extend(imm, 21);
}

#endif