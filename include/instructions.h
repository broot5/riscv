#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include <unistd.h>

#include "cpu.h"
#include "syscall.h"
#include "utils.h"

typedef void (*InstructionHandler)(uint32_t, CPU_t *);

static inline void handle_lui(uint32_t inst, CPU_t *cpu) {
  uint32_t imm = get_imm_u(inst);

  write_reg(cpu, get_rd(inst), imm);
  cpu->pc += 4;
}

static inline void handle_auipc(uint32_t inst, CPU_t *cpu) {
  uint32_t imm = get_imm_u(inst);

  write_reg(cpu, get_rd(inst), cpu->pc + imm);
  cpu->pc += 4;
}

static inline void handle_jal(uint32_t inst, CPU_t *cpu) {
  write_reg(cpu, get_rd(inst), cpu->pc + 4);
  cpu->pc += get_imm_j(inst);
}
static inline void handle_jalr(uint32_t inst, CPU_t *cpu) {
  uint32_t next_pc = cpu->pc + 4;
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  cpu->pc = (rs1_val + imm) & ~1;
  write_reg(cpu, get_rd(inst), next_pc);
}

static inline void handle_beq(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs1_val == rs2_val)
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bne(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs1_val != rs2_val)
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_blt(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if ((int32_t)rs1_val < (int32_t)rs2_val)
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bge(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if ((int32_t)rs1_val >= (int32_t)rs2_val)
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bltu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs1_val < rs2_val)
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}
static inline void handle_bgeu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs1_val >= rs2_val)
    cpu->pc += get_imm_b(inst);
  else
    cpu->pc += 4;
}

static inline void handle_lb(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), sign_extend(read_byte(cpu, rs1_val + imm), 8));
  cpu->pc += 4;
}
static inline void handle_lh(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), sign_extend(read_half(cpu, rs1_val + imm), 16));
  cpu->pc += 4;
}
static inline void handle_lw(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), read_word(cpu, rs1_val + imm));
  cpu->pc += 4;
}
static inline void handle_lbu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), read_byte(cpu, rs1_val + imm));
  cpu->pc += 4;
}
static inline void handle_lhu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), read_half(cpu, rs1_val + imm));
  cpu->pc += 4;
}

static inline void handle_sb(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int32_t imm = get_imm_s(inst);

  write_byte(cpu, rs1_val + imm, (uint8_t)rs2_val);
  cpu->pc += 4;
}
static inline void handle_sh(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int32_t imm = get_imm_s(inst);

  write_half(cpu, rs1_val + imm, (uint16_t)rs2_val);
  cpu->pc += 4;
}
static inline void handle_sw(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int32_t imm = get_imm_s(inst);

  write_word(cpu, rs1_val + imm, rs2_val);
  cpu->pc += 4;
}

static inline void handle_addi(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), rs1_val + imm);
  cpu->pc += 4;
}
static inline void handle_slti(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), (int32_t)rs1_val < imm);
  cpu->pc += 4;
}
static inline void handle_sltiu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), rs1_val < (uint32_t)imm);
  cpu->pc += 4;
}
static inline void handle_xori(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), rs1_val ^ imm);
  cpu->pc += 4;
}
static inline void handle_ori(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), rs1_val | imm);
  cpu->pc += 4;
}
static inline void handle_andi(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  int32_t imm = get_imm_i(inst);

  write_reg(cpu, get_rd(inst), rs1_val & imm);
  cpu->pc += 4;
}
static inline void handle_slli(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = get_imm_i(inst) & 0x1F;

  write_reg(cpu, get_rd(inst), rs1_val << shamt);
  cpu->pc += 4;
}
static inline void handle_srli(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = get_imm_i(inst) & 0x1F;

  write_reg(cpu, get_rd(inst), rs1_val >> shamt);
  cpu->pc += 4;
}
static inline void handle_srai(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t shamt = get_imm_i(inst) & 0x1F;

  write_reg(cpu, get_rd(inst), (uint32_t)((int32_t)rs1_val >> shamt));
  cpu->pc += 4;
}

static inline void handle_add(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val + rs2_val);
  cpu->pc += 4;
}
static inline void handle_sub(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val - rs2_val);
  cpu->pc += 4;
}
static inline void handle_sll(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val << (rs2_val & 0x1F));
  cpu->pc += 4;
}
static inline void handle_slt(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), (int32_t)rs1_val < (int32_t)rs2_val);
  cpu->pc += 4;
}
static inline void handle_sltu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val < rs2_val);
  cpu->pc += 4;
}
static inline void handle_xor(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val ^ rs2_val);
  cpu->pc += 4;
}
static inline void handle_srl(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val >> (rs2_val & 0x1F));
  cpu->pc += 4;
}
static inline void handle_sra(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst),
            (uint32_t)((int32_t)rs1_val >> (rs2_val & 0x1F)));
  cpu->pc += 4;
}
static inline void handle_or(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val | rs2_val);
  cpu->pc += 4;
}
static inline void handle_and(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val & rs2_val);
  cpu->pc += 4;
}

static inline void handle_fence(uint32_t inst, CPU_t *cpu) {
  (void)inst;

  cpu->pc += 4;
}

static inline void handle_ecall(uint32_t inst, CPU_t *cpu) {
  (void)inst;

  uint32_t syscall_num = read_reg(cpu, 17); // a7

  switch (syscall_num) {
  case 63: // SYS_read
    handle_sys_read(cpu);
    break;
  case 64: // SYS_write
    handle_sys_write(cpu);
    break;
  case 93: // SYS_exit
    handle_sys_exit(cpu);
    break;
  default:
    fprintf(stderr, "Error: Unknown syscall: %d\n", syscall_num);
    cpu->exit_code = 1;
    cpu->halt = true;
    break;
  }

  if (!cpu->halt) {
    cpu->pc += 4;
  }
}
static inline void handle_ebreak(uint32_t inst, CPU_t *cpu) {
  (void)inst;

  fprintf(stderr, "EBREAK executed at PC: 0x%08x\n", cpu->pc);
  cpu->halt = true;
}

static inline void handle_illegal_instruction(uint32_t inst, CPU_t *cpu) {
  fprintf(stderr, "Error: Illegal instruction at PC: 0x%08x, Inst: 0x%08x\n",
          cpu->pc, inst);
  cpu->exit_code = 1;
  cpu->halt = true;
}

// RV32M
static inline void handle_mul(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  write_reg(cpu, get_rd(inst), rs1_val * rs2_val);
  cpu->pc += 4;
}
static inline void handle_mulh(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int64_t result = (int64_t)(int32_t)rs1_val * (int64_t)(int32_t)rs2_val;

  write_reg(cpu, get_rd(inst), (uint32_t)(result >> 32));
  cpu->pc += 4;
}
static inline void handle_mulhsu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  int64_t result = (int64_t)(int32_t)rs1_val * (uint64_t)rs2_val;

  write_reg(cpu, get_rd(inst), (uint32_t)(result >> 32));
  cpu->pc += 4;
}
static inline void handle_mulhu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));
  uint64_t result = (uint64_t)rs1_val * (uint64_t)rs2_val;

  write_reg(cpu, get_rd(inst), (uint32_t)(result >> 32));
  cpu->pc += 4;
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
  cpu->pc += 4;
}
static inline void handle_divu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs2_val == 0) {
    write_reg(cpu, get_rd(inst), UINT32_MAX);
  } else {
    write_reg(cpu, get_rd(inst), rs1_val / rs2_val);
  }
  cpu->pc += 4;
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
  cpu->pc += 4;
}
static inline void handle_remu(uint32_t inst, CPU_t *cpu) {
  uint32_t rs1_val = read_reg(cpu, get_rs1(inst));
  uint32_t rs2_val = read_reg(cpu, get_rs2(inst));

  if (rs2_val == 0) {
    write_reg(cpu, get_rd(inst), rs1_val);
  } else {
    write_reg(cpu, get_rd(inst), rs1_val % rs2_val);
  }
  cpu->pc += 4;
}

#endif