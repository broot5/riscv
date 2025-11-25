#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>

#include "cpu.h"
#include "instructions.h"
#include "opcodes.h"
#include "utils.h"

static inline void handle_op_imm_001(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == 0b0000000)
    handle_slli(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_op_imm_101(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_srli(inst, cpu);
    break;
  case 0b0100000:
    handle_srai(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_000(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_add(inst, cpu);
    break;
  case 0b0100000:
    handle_sub(inst, cpu);
    break;
  case 0b0000001:
    handle_mul(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_001(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_sll(inst, cpu);
    break;
  case 0b0000001:
    handle_mulh(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_010(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_slt(inst, cpu);
    break;
  case 0b0000001:
    handle_mulhsu(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_011(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_sltu(inst, cpu);
    break;
  case 0b0000001:
    handle_mulhu(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_100(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_xor(inst, cpu);
    break;
  case 0b0000001:
    handle_div(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_101(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_srl(inst, cpu);
    break;
  case 0b0100000:
    handle_sra(inst, cpu);
    break;
  case 0b0000001:
    handle_divu(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_110(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_or(inst, cpu);
    break;
  case 0b0000001:
    handle_rem(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_op_111(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    handle_and(inst, cpu);
    break;
  case 0b0000001:
    handle_remu(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_system_000(uint32_t inst, CPU_t *cpu) {
  if (get_rd(inst) == 0 && get_rs1(inst) == 0) {
    switch (get_imm_i(inst)) {
    case 0:
      handle_ecall(inst, cpu);
      break;
    case 1:
      handle_ebreak(inst, cpu);
      break;
    default:
      handle_illegal_instruction(inst, cpu);
      break;
    }
  } else {
    handle_illegal_instruction(inst, cpu);
  }
}

typedef void (*InstructionHandler)(uint32_t, CPU_t *);

static inline void
init_dispatch_table(InstructionHandler dispatch_table[128][8]) {
  for (int i = 0; i < 128; i++) {
    for (int j = 0; j < 8; j++) {
      dispatch_table[i][j] = handle_illegal_instruction;
    }
  }

  for (int i = 0; i < 8; i++) {
    dispatch_table[OPCODE_LUI][i] = handle_lui;
    dispatch_table[OPCODE_AUIPC][i] = handle_auipc;
    dispatch_table[OPCODE_JAL][i] = handle_jal;
  }

  dispatch_table[OPCODE_JALR][0] = handle_jalr;

  dispatch_table[OPCODE_BRANCH][0b000] = handle_beq;
  dispatch_table[OPCODE_BRANCH][0b001] = handle_bne;
  dispatch_table[OPCODE_BRANCH][0b100] = handle_blt;
  dispatch_table[OPCODE_BRANCH][0b101] = handle_bge;
  dispatch_table[OPCODE_BRANCH][0b110] = handle_bltu;
  dispatch_table[OPCODE_BRANCH][0b111] = handle_bgeu;

  dispatch_table[OPCODE_LOAD][0b000] = handle_lb;
  dispatch_table[OPCODE_LOAD][0b001] = handle_lh;
  dispatch_table[OPCODE_LOAD][0b010] = handle_lw;
  dispatch_table[OPCODE_LOAD][0b100] = handle_lbu;
  dispatch_table[OPCODE_LOAD][0b101] = handle_lhu;

  dispatch_table[OPCODE_STORE][0b000] = handle_sb;
  dispatch_table[OPCODE_STORE][0b001] = handle_sh;
  dispatch_table[OPCODE_STORE][0b010] = handle_sw;

  dispatch_table[OPCODE_OP_IMM][0b000] = handle_addi;
  dispatch_table[OPCODE_OP_IMM][0b010] = handle_slti;
  dispatch_table[OPCODE_OP_IMM][0b011] = handle_sltiu;
  dispatch_table[OPCODE_OP_IMM][0b100] = handle_xori;
  dispatch_table[OPCODE_OP_IMM][0b110] = handle_ori;
  dispatch_table[OPCODE_OP_IMM][0b111] = handle_andi;
  dispatch_table[OPCODE_OP_IMM][0b001] = handle_op_imm_001;
  dispatch_table[OPCODE_OP_IMM][0b101] = handle_op_imm_101;

  dispatch_table[OPCODE_OP][0b000] = handle_op_000;
  dispatch_table[OPCODE_OP][0b001] = handle_op_001;
  dispatch_table[OPCODE_OP][0b010] = handle_op_010;
  dispatch_table[OPCODE_OP][0b011] = handle_op_011;
  dispatch_table[OPCODE_OP][0b100] = handle_op_100;
  dispatch_table[OPCODE_OP][0b101] = handle_op_101;
  dispatch_table[OPCODE_OP][0b110] = handle_op_110;
  dispatch_table[OPCODE_OP][0b111] = handle_op_111;

  dispatch_table[OPCODE_MISC_MEM][0b000] = handle_fence;

  dispatch_table[OPCODE_SYSTEM][0b000] = handle_system_000;
}

#endif