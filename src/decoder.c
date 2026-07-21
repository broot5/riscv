#include "decoder.h"

#include "instructions/instructions.h"
#include "instructions/instructions_m.h"
#include "opcodes.h"
#include "utils.h"

typedef void (*OpcodeHandler)(uint32_t inst, RvContext_t *context);

static void decode_jalr(uint32_t inst, RvContext_t *context) {
  if (get_funct3(inst) == 0)
    handle_jalr(inst, context);
  else
    handle_illegal_instruction(inst, context);
}

static void decode_branch(uint32_t inst, RvContext_t *context) {
  switch (get_funct3(inst)) {
  case 0b000:
    handle_beq(inst, context);
    break;
  case 0b001:
    handle_bne(inst, context);
    break;
  case 0b100:
    handle_blt(inst, context);
    break;
  case 0b101:
    handle_bge(inst, context);
    break;
  case 0b110:
    handle_bltu(inst, context);
    break;
  case 0b111:
    handle_bgeu(inst, context);
    break;
  default:
    handle_illegal_instruction(inst, context);
    break;
  }
}

static void decode_load(uint32_t inst, RvContext_t *context) {
  switch (get_funct3(inst)) {
  case 0b000:
    handle_lb(inst, context);
    break;
  case 0b001:
    handle_lh(inst, context);
    break;
  case 0b010:
    handle_lw(inst, context);
    break;
  case 0b100:
    handle_lbu(inst, context);
    break;
  case 0b101:
    handle_lhu(inst, context);
    break;
  default:
    handle_illegal_instruction(inst, context);
    break;
  }
}

static void decode_store(uint32_t inst, RvContext_t *context) {
  switch (get_funct3(inst)) {
  case 0b000:
    handle_sb(inst, context);
    break;
  case 0b001:
    handle_sh(inst, context);
    break;
  case 0b010:
    handle_sw(inst, context);
    break;
  default:
    handle_illegal_instruction(inst, context);
    break;
  }
}

static void decode_op_imm(uint32_t inst, RvContext_t *context) {
  switch (get_funct3(inst)) {
  case 0b000:
    handle_addi(inst, context);
    break;
  case 0b001:
    if (get_funct7(inst) == 0b0000000)
      handle_slli(inst, context);
    else
      handle_illegal_instruction(inst, context);
    break;
  case 0b010:
    handle_slti(inst, context);
    break;
  case 0b011:
    handle_sltiu(inst, context);
    break;
  case 0b100:
    handle_xori(inst, context);
    break;
  case 0b101:
    if (get_funct7(inst) == 0b0000000)
      handle_srli(inst, context);
    else if (get_funct7(inst) == 0b0100000)
      handle_srai(inst, context);
    else
      handle_illegal_instruction(inst, context);
    break;
  case 0b110:
    handle_ori(inst, context);
    break;
  case 0b111:
    handle_andi(inst, context);
    break;
  }
}

static void decode_m_extension(uint32_t inst, RvContext_t *context) {
  switch (get_funct3(inst)) {
  case 0b000:
    handle_mul(inst, context);
    break;
  case 0b001:
    handle_mulh(inst, context);
    break;
  case 0b010:
    handle_mulhsu(inst, context);
    break;
  case 0b011:
    handle_mulhu(inst, context);
    break;
  case 0b100:
    handle_div(inst, context);
    break;
  case 0b101:
    handle_divu(inst, context);
    break;
  case 0b110:
    handle_rem(inst, context);
    break;
  case 0b111:
    handle_remu(inst, context);
    break;
  }
}

static void decode_base_op(uint32_t inst, RvContext_t *context) {
  switch (get_funct3(inst)) {
  case 0b000:
    handle_add(inst, context);
    break;
  case 0b001:
    handle_sll(inst, context);
    break;
  case 0b010:
    handle_slt(inst, context);
    break;
  case 0b011:
    handle_sltu(inst, context);
    break;
  case 0b100:
    handle_xor(inst, context);
    break;
  case 0b101:
    handle_srl(inst, context);
    break;
  case 0b110:
    handle_or(inst, context);
    break;
  case 0b111:
    handle_and(inst, context);
    break;
  }
}

static void decode_op(uint32_t inst, RvContext_t *context) {
  switch (get_funct7(inst)) {
  case 0b0000000:
    decode_base_op(inst, context);
    break;
  case 0b0000001:
    decode_m_extension(inst, context);
    break;
  case 0b0100000:
    if (get_funct3(inst) == 0b000)
      handle_sub(inst, context);
    else if (get_funct3(inst) == 0b101)
      handle_sra(inst, context);
    else
      handle_illegal_instruction(inst, context);
    break;
  default:
    handle_illegal_instruction(inst, context);
    break;
  }
}

static void decode_misc_mem(uint32_t inst, RvContext_t *context) {
  if (get_funct3(inst) == 0b000)
    handle_fence(inst, context);
  else
    handle_illegal_instruction(inst, context);
}

static void decode_system(uint32_t inst, RvContext_t *context) {
  if (get_funct3(inst) != 0 || get_rd(inst) != 0 || get_rs1(inst) != 0) {
    handle_illegal_instruction(inst, context);
    return;
  }

  switch (get_imm_i(inst)) {
  case 0:
    handle_ecall(inst, context);
    break;
  case 1:
    handle_ebreak(inst, context);
    break;
  default:
    handle_illegal_instruction(inst, context);
    break;
  }
}

static const OpcodeHandler opcode_table[128] = {
    [OPCODE_LUI] = handle_lui,       [OPCODE_AUIPC] = handle_auipc,
    [OPCODE_JAL] = handle_jal,       [OPCODE_JALR] = decode_jalr,
    [OPCODE_BRANCH] = decode_branch, [OPCODE_LOAD] = decode_load,
    [OPCODE_STORE] = decode_store,   [OPCODE_OP_IMM] = decode_op_imm,
    [OPCODE_OP] = decode_op,         [OPCODE_MISC_MEM] = decode_misc_mem,
    [OPCODE_SYSTEM] = decode_system,
};

void decode_and_execute(uint32_t inst, RvContext_t *context) {
  OpcodeHandler handler = opcode_table[get_opcode(inst)];
  if (handler)
    handler(inst, context);
  else
    handle_illegal_instruction(inst, context);
}
