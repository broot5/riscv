#ifndef HANDLER_H
#define HANDLER_H

#include <stdint.h>

#include "cpu.h"
#include "utils.h"

typedef void (*InstructionHandler)(uint32_t, CPU_t *);

static inline void handle_lui(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), get_imm_u(inst));
  cpu->pc += 4;
}

static inline void handle_auipc(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), cpu->pc + get_imm_u(inst));
  cpu->pc += 4;
}

static inline void handle_jal(uint32_t inst, CPU_t *cpu) {
  cpu->pc += get_imm_j(inst);
}
static inline void handle_jalr(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), cpu->pc + 4);
  cpu->pc = (read_reg(cpu, get_rs1(inst)) + get_imm_i(inst)) & ~1;
}

static inline void handle_beq(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) == read_reg(cpu, get_rs2(inst)))
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bne(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) != read_reg(cpu, get_rs2(inst)))
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_blt(uint32_t inst, CPU_t *cpu) {
  if ((int32_t)read_reg(cpu, get_rs1(inst)) <
      (int32_t)read_reg(cpu, get_rs2(inst)))
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bge(uint32_t inst, CPU_t *cpu) {
  if ((int32_t)read_reg(cpu, get_rs1(inst)) >=
      (int32_t)read_reg(cpu, get_rs2(inst)))
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bltu(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) < read_reg(cpu, get_rs2(inst)))
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bgeu(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) >= read_reg(cpu, get_rs2(inst)))
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}

static inline void handle_lb(uint32_t inst, CPU_t *cpu) {
  write_reg(
      cpu, get_rd(inst),
      sign_extend(
          read_byte(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_i(inst)), 8));
  cpu->pc += 4;
}
static inline void handle_lh(uint32_t inst, CPU_t *cpu) {
  write_reg(
      cpu, get_rd(inst),
      sign_extend(
          read_half(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_i(inst)), 16));
  cpu->pc += 4;
}
static inline void handle_lw(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_word(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_i(inst)));
  cpu->pc += 4;
}
static inline void handle_lbu(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_byte(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_i(inst)));
  cpu->pc += 4;
}
static inline void handle_lhu(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_half(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_i(inst)));
  cpu->pc += 4;
}

static inline void handle_sb(uint32_t inst, CPU_t *cpu) {
  write_byte(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_s(inst),
             (uint8_t)read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_sh(uint32_t inst, CPU_t *cpu) {
  write_half(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_s(inst),
             (uint16_t)read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_sw(uint32_t inst, CPU_t *cpu) {
  write_word(cpu, read_reg(cpu, get_rs1(inst)) + get_imm_s(inst),
             read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}

static inline void handle_addi(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), read_reg(cpu, get_rs1(inst)) + get_imm_i(inst));
  cpu->pc += 4;
}
static inline void handle_slti(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            ((int32_t)read_reg(cpu, get_rs1(inst)) < get_imm_i(inst)));
  cpu->pc += 4;
}
static inline void handle_sltiu(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            (read_reg(cpu, get_rs1(inst)) < (uint32_t)get_imm_i(inst)));
  cpu->pc += 4;
}
static inline void handle_xori(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), read_reg(cpu, get_rs1(inst)) ^ get_imm_i(inst));
  cpu->pc += 4;
}
static inline void handle_ori(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), read_reg(cpu, get_rs1(inst)) | get_imm_i(inst));
  cpu->pc += 4;
}
static inline void handle_andi(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), read_reg(cpu, get_rs1(inst)) & get_imm_i(inst));
  cpu->pc += 4;
}
static inline void handle_slli(uint32_t inst, CPU_t *cpu) {
  int32_t shamt = get_imm_i(inst) & 0x1F;
  write_reg(cpu, get_rd(inst), read_reg(cpu, get_rs1(inst)) << shamt);
  cpu->pc += 4;
}
static inline void handle_srli(uint32_t inst, CPU_t *cpu) {
  int32_t shamt = get_imm_i(inst) & 0x1F;
  write_reg(cpu, get_rd(inst), read_reg(cpu, get_rs1(inst)) >> shamt);
  cpu->pc += 4;
}
static inline void handle_srai(uint32_t inst, CPU_t *cpu) {
  int32_t shamt = get_imm_i(inst) & 0x1F;
  write_reg(cpu, get_rd(inst),
            (uint32_t)((int32_t)read_reg(cpu, get_rs1(inst)) >> shamt));
  cpu->pc += 4;
}

static inline void handle_add(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) + read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_sub(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) - read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_sll(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) << read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_slt(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            (int32_t)read_reg(cpu, get_rs1(inst)) <
                (int32_t)read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_sltu(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) < read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_xor(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) ^ read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_srl(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) >> read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_sra(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            (uint32_t)((int32_t)read_reg(cpu, get_rs1(inst)) >>
                       (int32_t)read_reg(cpu, get_rs2(inst))));
  cpu->pc += 4;
}
static inline void handle_or(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) | read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}
static inline void handle_and(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) & read_reg(cpu, get_rs2(inst)));
  cpu->pc += 4;
}

static inline void handle_fence(uint32_t inst, CPU_t *cpu) { cpu->pc += 4; }
static inline void handle_ecall(uint32_t inst, CPU_t *cpu) { cpu->halt = true; }
static inline void handle_ebreak(uint32_t inst, CPU_t *cpu) {
  cpu->halt = true;
}

static inline void handle_illegal_instruction(uint32_t inst, CPU_t *cpu) {
  cpu->halt = true;
}

#endif