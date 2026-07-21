#include "compressed_decoder.h"

#include "opcodes.h"
#include "utils.h"

static uint8_t get_c_rd(uint16_t inst) { return (inst >> 7) & 0x1F; }

static uint8_t get_c_rs2(uint16_t inst) { return (inst >> 2) & 0x1F; }

static uint8_t get_c_rd_prime(uint16_t inst) { return (inst >> 2) & 0x7; }

static uint8_t get_c_rs1_prime(uint16_t inst) { return (inst >> 7) & 0x7; }

static uint8_t get_c_rd_prime_reg(uint16_t inst) {
  return 8 + get_c_rd_prime(inst);
}

static uint8_t get_c_rs1_prime_reg(uint16_t inst) {
  return 8 + get_c_rs1_prime(inst);
}

static uint8_t get_c_rs2_prime_reg(uint16_t inst) {
  return 8 + get_c_rd_prime(inst);
}

static int32_t get_imm_ci(uint16_t c_inst) {
  uint32_t imm = ((c_inst >> 2) & 0x1F) | ((c_inst >> 12) & 0x1) << 5;
  return sign_extend(imm, 6);
}

static int32_t get_imm_cj(uint16_t c_inst) {
  uint32_t imm = ((c_inst >> 3) & 0x7) << 1 |
                 ((c_inst >> 11) & 0x1) << 4 |
                 ((c_inst >> 2) & 0x1) << 5 |
                 ((c_inst >> 7) & 0x1) << 6 |
                 ((c_inst >> 6) & 0x1) << 7 |
                 ((c_inst >> 9) & 0x3) << 8 |
                 ((c_inst >> 8) & 0x1) << 10 |
                 ((c_inst >> 12) & 0x1) << 11;
  return sign_extend(imm, 12);
}

static int32_t get_imm_cb(uint16_t c_inst) {
  uint32_t imm = ((c_inst >> 3) & 0x3) << 1 |
                 ((c_inst >> 10) & 0x3) << 3 |
                 ((c_inst >> 2) & 0x1) << 5 |
                 ((c_inst >> 5) & 0x3) << 6 |
                 ((c_inst >> 12) & 0x1) << 8;
  return sign_extend(imm, 9);
}

static uint32_t get_imm_cl_cs(uint16_t c_inst) {
  return ((c_inst >> 6) & 0x1) | ((c_inst >> 10) & 0x7) << 1 |
         ((c_inst >> 5) & 0x1) << 4;
}

static uint32_t expand_illegal(uint16_t c_inst) {
  (void)c_inst;
  return 0;
}

static uint32_t expand_ciw_addi4spn(uint16_t c_inst) {
  uint8_t rd = get_c_rd_prime_reg(c_inst);
  uint32_t imm = ((c_inst >> 6) & 0x1) | ((c_inst >> 5) & 0x1) << 1 |
                 ((c_inst >> 11) & 0x3) << 2 |
                 ((c_inst >> 7) & 0xF) << 4;
  if (imm == 0)
    return 0;

  return build_i_type(OPCODE_OP_IMM, rd, 0b000, 2, imm << 2);
}

static uint32_t expand_cl_lw(uint16_t c_inst) {
  uint8_t rd = get_c_rd_prime_reg(c_inst);
  uint8_t rs1 = get_c_rs1_prime_reg(c_inst);
  uint32_t imm = get_imm_cl_cs(c_inst);
  return build_i_type(OPCODE_LOAD, rd, 0b010, rs1, imm << 2);
}

static uint32_t expand_cs_sw(uint16_t c_inst) {
  uint8_t rs2 = get_c_rs2_prime_reg(c_inst);
  uint8_t rs1 = get_c_rs1_prime_reg(c_inst);
  uint32_t imm = get_imm_cl_cs(c_inst);
  return build_s_type(OPCODE_STORE, 0b010, rs1, rs2, imm << 2);
}

static uint32_t expand_ci_addi(uint16_t c_inst) {
  uint8_t rd = get_c_rd(c_inst);
  return build_i_type(OPCODE_OP_IMM, rd, 0b000, rd, get_imm_ci(c_inst));
}

static uint32_t expand_cj_jal(uint16_t c_inst) {
  return build_j_type(OPCODE_JAL, 1, get_imm_cj(c_inst));
}

static uint32_t expand_ci_li(uint16_t c_inst) {
  return build_i_type(OPCODE_OP_IMM, get_c_rd(c_inst), 0b000, 0,
                      get_imm_ci(c_inst));
}

static uint32_t expand_ci_lui_addi16sp(uint16_t c_inst) {
  uint8_t rd = get_c_rd(c_inst);
  int32_t imm;

  if (rd == 2) {
    imm = ((c_inst >> 6) & 0x1) | ((c_inst >> 2) & 0x1) << 1 |
          ((c_inst >> 5) & 0x1) << 2 | ((c_inst >> 3) & 0x3) << 3 |
          ((c_inst >> 12) & 0x1) << 5;
    imm = sign_extend((uint32_t)imm, 6);
    if (imm == 0)
      return 0;
    return build_i_type(OPCODE_OP_IMM, 2, 0b000, 2, imm * 16);
  }

  imm = get_imm_ci(c_inst);
  if (imm == 0)
    return 0;
  return build_u_type(OPCODE_LUI, rd, imm * 4096);
}

static uint32_t expand_cb_ca_misc_alu(uint16_t c_inst) {
  uint8_t sub_funct = (c_inst >> 10) & 0x3;
  uint8_t rs1_prime = get_c_rs1_prime_reg(c_inst);
  int32_t imm = get_imm_ci(c_inst);

  switch (sub_funct) {
  case 0b00:
    return build_i_type(OPCODE_OP_IMM, rs1_prime, 0b101, rs1_prime,
                        imm & 0x3F);
  case 0b01:
    return build_i_type(OPCODE_OP_IMM, rs1_prime, 0b101, rs1_prime,
                        (imm & 0x3F) | 0x400);
  case 0b10:
    return build_i_type(OPCODE_OP_IMM, rs1_prime, 0b111, rs1_prime, imm);
  case 0b11: {
    uint8_t rs2 = get_c_rs2_prime_reg(c_inst);
    uint8_t op_funct = ((c_inst >> 5) & 0x3) |
                       ((c_inst >> 12) & 0x1) << 2;
    switch (op_funct) {
    case 0:
      return build_r_type(OPCODE_OP, rs1_prime, 0b000, rs1_prime, rs2, 0x20);
    case 1:
      return build_r_type(OPCODE_OP, rs1_prime, 0b100, rs1_prime, rs2, 0x00);
    case 2:
      return build_r_type(OPCODE_OP, rs1_prime, 0b110, rs1_prime, rs2, 0x00);
    case 3:
      return build_r_type(OPCODE_OP, rs1_prime, 0b111, rs1_prime, rs2, 0x00);
    default:
      return 0;
    }
  }
  default:
    return 0;
  }
}

static uint32_t expand_cj_j(uint16_t c_inst) {
  return build_j_type(OPCODE_JAL, 0, get_imm_cj(c_inst));
}

static uint32_t expand_cb_beqz(uint16_t c_inst) {
  return build_b_type(OPCODE_BRANCH, 0b000, get_c_rs1_prime_reg(c_inst), 0,
                      get_imm_cb(c_inst));
}

static uint32_t expand_cb_bnez(uint16_t c_inst) {
  return build_b_type(OPCODE_BRANCH, 0b001, get_c_rs1_prime_reg(c_inst), 0,
                      get_imm_cb(c_inst));
}

static uint32_t expand_ci_slli(uint16_t c_inst) {
  uint8_t rd = get_c_rd(c_inst);
  uint32_t imm = get_imm_ci(c_inst) & 0x3F;
  return build_i_type(OPCODE_OP_IMM, rd, 0b001, rd, imm);
}

static uint32_t expand_ci_lwsp(uint16_t c_inst) {
  uint8_t rd = get_c_rd(c_inst);
  uint32_t imm = ((c_inst >> 4) & 0x7) |
                 ((c_inst >> 12) & 0x1) << 3 |
                 ((c_inst >> 2) & 0x3) << 4;
  if (rd == 0)
    return 0;
  return build_i_type(OPCODE_LOAD, rd, 0b010, 2, imm << 2);
}

static uint32_t expand_cr_jr_mv_add(uint16_t c_inst) {
  uint8_t rd = get_c_rd(c_inst);
  uint8_t rs2 = get_c_rs2(c_inst);

  if ((c_inst >> 12) & 0x1) {
    if (rs2 == 0) {
      if (rd == 0)
        return build_i_type(OPCODE_SYSTEM, 0, 0b000, 0, 1);
      return build_i_type(OPCODE_JALR, 1, 0b000, rd, 0);
    }
    return build_r_type(OPCODE_OP, rd, 0b000, rd, rs2, 0x00);
  }

  if (rs2 == 0) {
    if (rd == 0)
      return 0;
    return build_i_type(OPCODE_JALR, 0, 0b000, rd, 0);
  }
  return build_r_type(OPCODE_OP, rd, 0b000, 0, rs2, 0x00);
}

static uint32_t expand_css_swsp(uint16_t c_inst) {
  uint8_t rs2 = get_c_rs2(c_inst);
  uint32_t imm = ((c_inst >> 9) & 0xF) |
                 ((c_inst >> 7) & 0x3) << 4;
  return build_s_type(OPCODE_STORE, 0b010, 2, rs2, imm << 2);
}

uint32_t expand_compressed(uint16_t c_inst) {
  uint8_t op = c_inst & 0b11;
  uint8_t funct3 = (c_inst >> 13) & 0b111;

  switch (op) {
  case 0b00:
    switch (funct3) {
    case 0b000:
      return expand_ciw_addi4spn(c_inst);
    case 0b010:
      return expand_cl_lw(c_inst);
    case 0b110:
      return expand_cs_sw(c_inst);
    default:
      return expand_illegal(c_inst);
    }
  case 0b01:
    switch (funct3) {
    case 0b000:
      return expand_ci_addi(c_inst);
    case 0b001:
      return expand_cj_jal(c_inst);
    case 0b010:
      return expand_ci_li(c_inst);
    case 0b011:
      return expand_ci_lui_addi16sp(c_inst);
    case 0b100:
      return expand_cb_ca_misc_alu(c_inst);
    case 0b101:
      return expand_cj_j(c_inst);
    case 0b110:
      return expand_cb_beqz(c_inst);
    case 0b111:
      return expand_cb_bnez(c_inst);
    default:
      return expand_illegal(c_inst);
    }
  case 0b10:
    switch (funct3) {
    case 0b000:
      return expand_ci_slli(c_inst);
    case 0b010:
      return expand_ci_lwsp(c_inst);
    case 0b100:
      return expand_cr_jr_mv_add(c_inst);
    case 0b110:
      return expand_css_swsp(c_inst);
    default:
      return expand_illegal(c_inst);
    }
  default:
    return expand_illegal(c_inst);
  }
}
