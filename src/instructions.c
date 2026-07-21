#include "instructions/instructions.h"

#include <stdint.h>
#include <stdio.h>

#include "syscall.h"
#include "utils.h"

void handle_lui(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), get_imm_u(inst));
}

void handle_auipc(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), cpu->pc + get_imm_u(inst));
}

void handle_jal(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), cpu->pc + cpu->current_inst_len);
  cpu->next_pc = cpu->pc + get_imm_j(inst);
}

void handle_jalr(uint32_t inst, CPU_t *cpu) {
  uint32_t next_pc = cpu->pc + cpu->current_inst_len;
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  cpu->next_pc = (rs1_val + imm) & ~1;
  write_reg(cpu, get_rd(inst), next_pc);
}

void handle_beq(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) == read_reg(cpu, get_rs2(inst)))
    cpu->next_pc = cpu->pc + get_imm_b(inst);
}

void handle_bne(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) != read_reg(cpu, get_rs2(inst)))
    cpu->next_pc = cpu->pc + get_imm_b(inst);
}

void handle_blt(uint32_t inst, CPU_t *cpu) {
  int32_t rs1_val = (int32_t)read_reg(cpu, get_rs1(inst));
  int32_t rs2_val = (int32_t)read_reg(cpu, get_rs2(inst));
  if (rs1_val < rs2_val)
    cpu->next_pc = cpu->pc + get_imm_b(inst);
}

void handle_bge(uint32_t inst, CPU_t *cpu) {
  int32_t rs1_val = (int32_t)read_reg(cpu, get_rs1(inst));
  int32_t rs2_val = (int32_t)read_reg(cpu, get_rs2(inst));
  if (rs1_val >= rs2_val)
    cpu->next_pc = cpu->pc + get_imm_b(inst);
}

void handle_bltu(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) < read_reg(cpu, get_rs2(inst)))
    cpu->next_pc = cpu->pc + get_imm_b(inst);
}

void handle_bgeu(uint32_t inst, CPU_t *cpu) {
  if (read_reg(cpu, get_rs1(inst)) >= read_reg(cpu, get_rs2(inst)))
    cpu->next_pc = cpu->pc + get_imm_b(inst);
}

void handle_lb(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_i(inst);
  write_reg(cpu, get_rd(inst), sign_extend(read_byte(cpu, addr), 8));
}

void handle_lh(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_i(inst);
  write_reg(cpu, get_rd(inst), sign_extend(read_half(cpu, addr), 16));
}

void handle_lw(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_i(inst);
  write_reg(cpu, get_rd(inst), read_word(cpu, addr));
}

void handle_lbu(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_i(inst);
  write_reg(cpu, get_rd(inst), read_byte(cpu, addr));
}

void handle_lhu(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_i(inst);
  write_reg(cpu, get_rd(inst), read_half(cpu, addr));
}

void handle_sb(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_s(inst);
  write_byte(cpu, addr, (uint8_t)read_reg(cpu, get_rs2(inst)));
}

void handle_sh(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_s(inst);
  write_half(cpu, addr, (uint16_t)read_reg(cpu, get_rs2(inst)));
}

void handle_sw(uint32_t inst, CPU_t *cpu) {
  uint32_t addr = read_reg(cpu, get_rs1(inst)) + get_imm_s(inst);
  write_word(cpu, addr, read_reg(cpu, get_rs2(inst)));
}

void handle_addi(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) + get_imm_i(inst));
}

void handle_slti(uint32_t inst, CPU_t *cpu) {
  int32_t rs1_val = (int32_t)read_reg(cpu, get_rs1(inst));
  write_reg(cpu, get_rd(inst), rs1_val < get_imm_i(inst));
}

void handle_sltiu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  write_reg(cpu, get_rd(inst), rs1_val < (uint32_t)get_imm_i(inst));
}

void handle_xori(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) ^ get_imm_i(inst));
}

void handle_ori(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) | get_imm_i(inst));
}

void handle_andi(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) & get_imm_i(inst));
}

void handle_slli(uint32_t inst, CPU_t *cpu) {
  uint32_t value = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = get_imm_i(inst) & 0x1F;
  write_reg(cpu, get_rd(inst), value << shamt);
}

void handle_srli(uint32_t inst, CPU_t *cpu) {
  uint32_t value = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = get_imm_i(inst) & 0x1F;
  write_reg(cpu, get_rd(inst), value >> shamt);
}

void handle_srai(uint32_t inst, CPU_t *cpu) {
  int32_t value = (int32_t)read_reg(cpu, get_rs1(inst));
  uint32_t shamt = get_imm_i(inst) & 0x1F;
  write_reg(cpu, get_rd(inst), (uint32_t)(value >> shamt));
}

void handle_add(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) + read_reg(cpu, get_rs2(inst)));
}

void handle_sub(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) - read_reg(cpu, get_rs2(inst)));
}

void handle_sll(uint32_t inst, CPU_t *cpu) {
  uint32_t value = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = read_reg(cpu, get_rs2(inst)) & 0x1F;
  write_reg(cpu, get_rd(inst), value << shamt);
}

void handle_slt(uint32_t inst, CPU_t *cpu) {
  int32_t rs1_val = (int32_t)read_reg(cpu, get_rs1(inst));
  int32_t rs2_val = (int32_t)read_reg(cpu, get_rs2(inst));
  write_reg(cpu, get_rd(inst), rs1_val < rs2_val);
}

void handle_sltu(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) < read_reg(cpu, get_rs2(inst)));
}

void handle_xor(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) ^ read_reg(cpu, get_rs2(inst)));
}

void handle_srl(uint32_t inst, CPU_t *cpu) {
  uint32_t value = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = read_reg(cpu, get_rs2(inst)) & 0x1F;
  write_reg(cpu, get_rd(inst), value >> shamt);
}

void handle_sra(uint32_t inst, CPU_t *cpu) {
  int32_t value = (int32_t)read_reg(cpu, get_rs1(inst));
  uint32_t shamt = read_reg(cpu, get_rs2(inst)) & 0x1F;
  write_reg(cpu, get_rd(inst), (uint32_t)(value >> shamt));
}

void handle_or(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) | read_reg(cpu, get_rs2(inst)));
}

void handle_and(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst),
            read_reg(cpu, get_rs1(inst)) & read_reg(cpu, get_rs2(inst)));
}

void handle_fence(uint32_t inst, CPU_t *cpu) {
  (void)inst;
  (void)cpu;
}

void handle_ecall(uint32_t inst, CPU_t *cpu) {
  (void)inst;

  switch (read_reg(cpu, 17)) {
  case 63:
    handle_sys_read(cpu);
    break;
  case 64:
    handle_sys_write(cpu);
    break;
  case 93:
    handle_sys_exit(cpu);
    break;
  case 214:
    handle_sys_brk(cpu);
    break;
  default:
    fprintf(stderr, "Error: Unknown syscall: %u\n", read_reg(cpu, 17));
    cpu->exit_code = 1;
    cpu->halt = true;
    break;
  }
}

void handle_ebreak(uint32_t inst, CPU_t *cpu) {
  (void)inst;
  fprintf(stderr, "EBREAK executed at PC: 0x%08x\n", cpu->pc);
  cpu->halt = true;
}

void handle_illegal_instruction(uint32_t inst, CPU_t *cpu) {
  fprintf(stderr, "Error: Illegal instruction at PC: 0x%08x, Inst: 0x%08x\n",
          cpu->pc, inst);
  cpu->exit_code = 1;
  cpu->halt = true;
}
