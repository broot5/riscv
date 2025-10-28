#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

#include "code_defines.h"
#include "cpu.h"
#include "utils.h"

typedef enum instruction {
  INST_UNKNOWN,
  INST_LUI,
  INST_AUIPC,
  INST_JAL,
  INST_JALR,

  INST_BEQ,
  INST_BNE,
  INST_BLT,
  INST_BGE,
  INST_BLTU,
  INST_BGEU,

  INST_LB,
  INST_LH,
  INST_LW,
  INST_LBU,
  INST_LHU,

  INST_SB,
  INST_SH,
  INST_SW,

  INST_ADDI,
  INST_SLTI,
  INST_SLTIU,
  INST_XORI,
  INST_ORI,
  INST_ANDI,
  INST_SLLI,
  INST_SRLI,
  INST_SRAI,

  INST_ADD,
  INST_SUB,
  INST_SLL,
  INST_SLT,
  INST_SLTU,
  INST_XOR,
  INST_SRL,
  INST_SRA,
  INST_OR,
  INST_AND,

  INST_FENCE,

  INST_ECALL,
  INST_EBREAK,
} instruction_t;

typedef struct decoded_instruction {
  instruction_t type;
  uint32_t opcode;
  uint32_t funct3;
  uint32_t funct7;
  uint32_t rd;
  uint32_t rs1;
  uint32_t rs2;
  int32_t imm;
} decoded_instruction_t;

static inline decoded_instruction_t decode_instruction(uint32_t instruction) {
  decoded_instruction_t decoded_inst = {.type = INST_UNKNOWN,
                                        .opcode = 0,
                                        .funct3 = 0,
                                        .funct7 = 0,
                                        .rd = 0,
                                        .rs1 = 0,
                                        .rs2 = 0,
                                        .imm = 0};

  decoded_inst.opcode = get_bits(instruction, 6, 0);
  decoded_inst.funct3 = get_bits(instruction, 14, 12);
  decoded_inst.funct7 = get_bits(instruction, 31, 25);
  decoded_inst.rd = get_bits(instruction, 11, 7);
  decoded_inst.rs1 = get_bits(instruction, 19, 15);
  decoded_inst.rs2 = get_bits(instruction, 24, 20);

  switch (decoded_inst.opcode) {
  case OPCODE_LUI:
    decoded_inst.type = INST_LUI;
    decoded_inst.imm = get_imm_u(instruction);
    break;
  case OPCODE_AUIPC:
    decoded_inst.type = INST_AUIPC;
    decoded_inst.imm = get_imm_u(instruction);
    break;
  case OPCODE_JAL:
    decoded_inst.type = INST_JAL;
    decoded_inst.imm = get_imm_j(instruction);
    break;
  case OPCODE_JALR:
    decoded_inst.type = INST_JALR;
    decoded_inst.imm = get_imm_i(instruction);
    break;
  case OPCODE_BRANCH:
    decoded_inst.imm = get_imm_b(instruction);
    switch (decoded_inst.funct3) {
    case FUNCT3_BEQ:
      decoded_inst.type = INST_BEQ;
      break;
    case FUNCT3_BNE:
      decoded_inst.type = INST_BNE;
      break;
    case FUNCT3_BLT:
      decoded_inst.type = INST_BLT;
      break;
    case FUNCT3_BGE:
      decoded_inst.type = INST_BGE;
      break;
    case FUNCT3_BLTU:
      decoded_inst.type = INST_BLTU;
      break;
    case FUNCT3_BGEU:
      decoded_inst.type = INST_BGEU;
      break;
    }
    break;
  case OPCODE_LOAD:
    decoded_inst.imm = get_imm_i(instruction);
    switch (decoded_inst.funct3) {
    case FUNCT3_LB:
      decoded_inst.type = INST_LB;
      break;
    case FUNCT3_LH:
      decoded_inst.type = INST_LH;
      break;
    case FUNCT3_LW:
      decoded_inst.type = INST_LW;
      break;
    case FUNCT3_LBU:
      decoded_inst.type = INST_LBU;
      break;
    case FUNCT3_LHU:
      decoded_inst.type = INST_LHU;
      break;
    }
    break;
  case OPCODE_STORE:
    decoded_inst.imm = get_imm_s(instruction);
    switch (decoded_inst.funct3) {
    case FUNCT3_SB:
      decoded_inst.type = INST_SB;
      break;
    case FUNCT3_SH:
      decoded_inst.type = INST_SH;
      break;
    case FUNCT3_SW:
      decoded_inst.type = INST_SW;
      break;
    }
    break;
  case OPCODE_OP_IMM:
    decoded_inst.imm = get_imm_i(instruction); // Immediate can be used as shamt
    switch (decoded_inst.funct3) {
    case FUNCT3_ADDI:
      decoded_inst.type = INST_ADDI;
      break;
    case FUNCT3_SLTI:
      decoded_inst.type = INST_SLTI;
      break;
    case FUNCT3_SLTIU:
      decoded_inst.type = INST_SLTIU;
      break;
    case FUNCT3_XORI:
      decoded_inst.type = INST_XORI;
      break;
    case FUNCT3_ORI:
      decoded_inst.type = INST_ORI;
      break;
    case FUNCT3_ANDI:
      decoded_inst.type = INST_ANDI;
      break;
    case FUNCT3_SLLI:
      if (decoded_inst.funct7 == FUNCT7_SLLI) {
        decoded_inst.type = INST_SLLI;
        decoded_inst.imm &= 0x1F; // 5bit
      }
      break;
    case FUNCT3_SRLI:
      if (decoded_inst.funct7 == FUNCT7_SRLI) {
        decoded_inst.type = INST_SRLI;
        decoded_inst.imm &= 0x1F;
      } else if (decoded_inst.funct7 == FUNCT7_SRAI) {
        decoded_inst.type = INST_SRAI;
        decoded_inst.imm &= 0x1F;
      }
      break;
    }
    break;
  case OPCODE_OP:
    switch (decoded_inst.funct3) {
    case FUNCT3_ADD:
      if (decoded_inst.funct7 == FUNCT7_ADD)
        decoded_inst.type = INST_ADD;
      else if (decoded_inst.funct7 == FUNCT7_SUB)
        decoded_inst.type = INST_SUB;
      break;
    case FUNCT3_SLL:
      decoded_inst.type = INST_SLL;
      break;
    case FUNCT3_SLT:
      decoded_inst.type = INST_SLT;
      break;
    case FUNCT3_SLTU:
      decoded_inst.type = INST_SLTU;
      break;
    case FUNCT3_XOR:
      decoded_inst.type = INST_XOR;
      break;
    case FUNCT3_SRL:
      if (decoded_inst.funct7 == FUNCT7_SRL)
        decoded_inst.type = INST_SRL;
      else if (decoded_inst.funct7 == FUNCT7_SRA)
        decoded_inst.type = INST_SRA;
      break;
    case FUNCT3_OR:
      decoded_inst.type = INST_OR;
      break;
    case FUNCT3_AND:
      decoded_inst.type = INST_AND;
      break;
    }
    break;
  case OPCODE_MISC_MEM:
    if (decoded_inst.funct3 == FUNCT3_FENCE) {
      decoded_inst.type = INST_FENCE;
      // uint32_t fence_imm = get_imm_i(instruction);
      // decoded_inst.pred = (fence_imm >> 8) & 0xF;
      // decoded_inst.succ = (fence_imm >> 4) & 0xF;
    }
    break;
  case OPCODE_SYSTEM:
    decoded_inst.imm = get_imm_i(instruction);
    switch (decoded_inst.funct3) {
    case FUNCT3_ECALL:
      if (decoded_inst.imm == 0) { // ECALL: imm = 0, rd = 0, rs1 = 0
        decoded_inst.type = INST_ECALL;
      } else if (decoded_inst.imm == 1) { // EBREAK: imm = 1, rd = 0, rs1 = 0
        decoded_inst.type = INST_EBREAK;
      }
      break;
    }
    break;
  default:
    break;
  }

  return decoded_inst;
}

static inline bool execute_instruction(decoded_instruction_t *inst,
                                       CPU_t *cpu) {
  uint32_t next_pc = cpu->pc + sizeof(uint32_t);
  uint32_t t;

  switch (inst->type) {
  case INST_LUI:
    write_reg(cpu, inst->rd, inst->imm);
    break;
  case INST_AUIPC:
    write_reg(cpu, inst->rd, cpu->pc + inst->imm);
    break;
  case INST_JAL:
    write_reg(cpu, inst->rd, cpu->pc + 4);
    next_pc = cpu->pc + inst->imm;
    break;
  case INST_JALR:
    t = cpu->pc + 4;
    next_pc = (read_reg(cpu, inst->rs1) + inst->imm) & ~1U;
    write_reg(cpu, inst->rd, t);
    break;
  case INST_BEQ:
    if (read_reg(cpu, inst->rs1) == read_reg(cpu, inst->rs2))
      next_pc = cpu->pc + inst->imm;
    break;
  case INST_BNE:
    if (read_reg(cpu, inst->rs1) != read_reg(cpu, inst->rs2))
      next_pc = cpu->pc + inst->imm;
    break;
  case INST_BLT:
    if ((int32_t)read_reg(cpu, inst->rs1) < (int32_t)read_reg(cpu, inst->rs2))
      next_pc = cpu->pc + inst->imm;
    break;
  case INST_BGE:
    if ((int32_t)read_reg(cpu, inst->rs1) >= (int32_t)read_reg(cpu, inst->rs2))
      next_pc = cpu->pc + inst->imm;
    break;
  case INST_BLTU:
    if (read_reg(cpu, inst->rs1) < read_reg(cpu, inst->rs2))
      next_pc = cpu->pc + inst->imm;
    break;
  case INST_BGEU:
    if (read_reg(cpu, inst->rs1) >= read_reg(cpu, inst->rs2))
      next_pc = cpu->pc + inst->imm;
    break;

  case INST_LB:
    write_reg(
        cpu, inst->rd,
        sign_extend(read_byte(cpu, read_reg(cpu, inst->rs1) + inst->imm), 8));
    break;
  case INST_LH:
    write_reg(
        cpu, inst->rd,
        sign_extend(read_half(cpu, read_reg(cpu, inst->rs1) + inst->imm), 16));
    break;
  case INST_LW:
    write_reg(cpu, inst->rd,
              read_word(cpu, read_reg(cpu, inst->rs1) + inst->imm));
    break;
  case INST_LBU:
    write_reg(cpu, inst->rd,
              read_byte(cpu, read_reg(cpu, inst->rs1) + inst->imm));
    break;
  case INST_LHU:
    write_reg(cpu, inst->rd,
              read_half(cpu, read_reg(cpu, inst->rs1) + inst->imm));
    break;

  case INST_SB:
    write_byte(cpu, read_reg(cpu, inst->rs1) + inst->imm,
               (uint8_t)read_reg(cpu, inst->rs2));
    break;
  case INST_SH:
    write_half(cpu, read_reg(cpu, inst->rs1) + inst->imm,
               (uint16_t)read_reg(cpu, inst->rs2));
    break;
  case INST_SW:
    write_word(cpu, read_reg(cpu, inst->rs1) + inst->imm,
               read_reg(cpu, inst->rs2));
    break;

  case INST_ADDI:
    write_reg(cpu, inst->rd, read_reg(cpu, inst->rs1) + inst->imm);
    break;
  case INST_SLTI:
    write_reg(cpu, inst->rd, ((int32_t)read_reg(cpu, inst->rs1) < inst->imm));
    break;
  case INST_SLTIU:
    write_reg(cpu, inst->rd, (read_reg(cpu, inst->rs1) < (uint32_t)inst->imm));
    break;
  case INST_XORI:
    write_reg(cpu, inst->rd, read_reg(cpu, inst->rs1) ^ inst->imm);
    break;
  case INST_ORI:
    write_reg(cpu, inst->rd, read_reg(cpu, inst->rs1) | inst->imm);
    break;
  case INST_ANDI:
    write_reg(cpu, inst->rd, read_reg(cpu, inst->rs1) & inst->imm);
    break;
  case INST_SLLI:
    write_reg(cpu, inst->rd, read_reg(cpu, inst->rs1) << inst->imm);
    break;
  case INST_SRLI:
    write_reg(cpu, inst->rd, read_reg(cpu, inst->rs1) >> inst->imm);
    break;
  case INST_SRAI:
    write_reg(cpu, inst->rd,
              (uint32_t)((int32_t)read_reg(cpu, inst->rs1) >> inst->imm));
    break;

  case INST_ADD:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) + read_reg(cpu, inst->rs2));
    break;
  case INST_SUB:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) - read_reg(cpu, inst->rs2));
    break;
  case INST_SLL:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) << read_reg(cpu, inst->rs2));
    break;
  case INST_SLT:
    write_reg(cpu, inst->rd,
              (int32_t)read_reg(cpu, inst->rs1) <
                  (int32_t)read_reg(cpu, inst->rs2));
    break;
  case INST_SLTU:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) < read_reg(cpu, inst->rs2));
    break;
  case INST_XOR:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) ^ read_reg(cpu, inst->rs2));
    break;
  case INST_SRL:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) >> read_reg(cpu, inst->rs2));
    break;
  case INST_SRA:
    write_reg(cpu, inst->rd,
              (uint32_t)((int32_t)read_reg(cpu, inst->rs1) >>
                         (int32_t)read_reg(cpu, inst->rs2)));
    break;
  case INST_OR:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) | read_reg(cpu, inst->rs2));
    break;
  case INST_AND:
    write_reg(cpu, inst->rd,
              read_reg(cpu, inst->rs1) & read_reg(cpu, inst->rs2));
    break;
  case INST_FENCE:
    break;
  case INST_ECALL:
  case INST_EBREAK:
    cpu->halt = true;
    break;

  case INST_UNKNOWN:
  default:
    cpu->halt = true;
    break;
  }

  if (!cpu->halt) {
    cpu->pc = next_pc;
  }

  return !cpu->halt;
}

#endif