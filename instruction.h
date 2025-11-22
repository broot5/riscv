#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

#include "code_defines.h"
#include "cpu.h"
#include "handler.h"
#include "utils.h"

static inline void handle_funct7_slli(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_SLLI)
    handle_slli(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_funct7_srli_srai(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case FUNCT7_SRLI:
    handle_srli(inst, cpu);
    break;
  case FUNCT7_SRAI:
    handle_srai(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_funct7_add_sub(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case FUNCT7_ADD:
    handle_add(inst, cpu);
    break;
  case FUNCT7_SUB:
    handle_sub(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_funct7_sll(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_SLL)
    handle_sll(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_funct7_slt(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_SLT)
    handle_slt(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_funct7_sltu(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_SLTU)
    handle_sltu(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_funct7_xor(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_XOR)
    handle_xor(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_funct7_srl_sra(uint32_t inst, CPU_t *cpu) {
  switch (get_funct7(inst)) {
  case FUNCT7_SRL:
    handle_srl(inst, cpu);
    break;
  case FUNCT7_SRA:
    handle_sra(inst, cpu);
    break;
  default:
    handle_illegal_instruction(inst, cpu);
    break;
  }
}

static inline void handle_funct7_or(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_OR)
    handle_or(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_funct7_and(uint32_t inst, CPU_t *cpu) {
  if (get_funct7(inst) == FUNCT7_AND)
    handle_and(inst, cpu);
  else
    handle_illegal_instruction(inst, cpu);
}

static inline void handle_ecall_ebreak(uint32_t inst, CPU_t *cpu) {
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

extern InstructionHandler dispatch_table[128][8];

static inline void init_dispatch_table() {
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

  dispatch_table[OPCODE_BRANCH][FUNCT3_BEQ] = handle_beq;
  dispatch_table[OPCODE_BRANCH][FUNCT3_BNE] = handle_bne;
  dispatch_table[OPCODE_BRANCH][FUNCT3_BLT] = handle_blt;
  dispatch_table[OPCODE_BRANCH][FUNCT3_BGE] = handle_bge;
  dispatch_table[OPCODE_BRANCH][FUNCT3_BLTU] = handle_bltu;
  dispatch_table[OPCODE_BRANCH][FUNCT3_BGEU] = handle_bgeu;

  dispatch_table[OPCODE_LOAD][FUNCT3_LB] = handle_lb;
  dispatch_table[OPCODE_LOAD][FUNCT3_LH] = handle_lh;
  dispatch_table[OPCODE_LOAD][FUNCT3_LW] = handle_lw;
  dispatch_table[OPCODE_LOAD][FUNCT3_LBU] = handle_lbu;
  dispatch_table[OPCODE_LOAD][FUNCT3_LHU] = handle_lhu;

  dispatch_table[OPCODE_STORE][FUNCT3_SB] = handle_sb;
  dispatch_table[OPCODE_STORE][FUNCT3_SH] = handle_sh;
  dispatch_table[OPCODE_STORE][FUNCT3_SW] = handle_sw;

  dispatch_table[OPCODE_OP_IMM][FUNCT3_ADDI] = handle_addi;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_SLTI] = handle_slti;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_SLTIU] = handle_sltiu;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_XORI] = handle_xori;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_ORI] = handle_ori;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_ANDI] = handle_andi;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_SLLI] = handle_funct7_slli;
  dispatch_table[OPCODE_OP_IMM][FUNCT3_SRLI_SRAI] = handle_funct7_srli_srai;

  dispatch_table[OPCODE_OP][FUNCT3_ADD_SUB] = handle_funct7_add_sub;
  dispatch_table[OPCODE_OP][FUNCT3_SLL] = handle_funct7_sll;
  dispatch_table[OPCODE_OP][FUNCT3_SLT] = handle_funct7_slt;
  dispatch_table[OPCODE_OP][FUNCT3_SLTU] = handle_funct7_sltu;
  dispatch_table[OPCODE_OP][FUNCT3_XOR] = handle_funct7_xor;
  dispatch_table[OPCODE_OP][FUNCT3_SRL_SRA] = handle_funct7_srl_sra;
  dispatch_table[OPCODE_OP][FUNCT3_OR] = handle_funct7_or;
  dispatch_table[OPCODE_OP][FUNCT3_AND] = handle_funct7_and;

  dispatch_table[OPCODE_MISC_MEM][FUNCT3_FENCE] = handle_fence;

  dispatch_table[OPCODE_SYSTEM][FUNCT3_ECALL_EBREAK] = handle_ecall_ebreak;
}

#endif