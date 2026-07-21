#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

#include "rv_context.h"

void handle_lui(uint32_t inst, RvContext_t *context);
void handle_auipc(uint32_t inst, RvContext_t *context);
void handle_jal(uint32_t inst, RvContext_t *context);
void handle_jalr(uint32_t inst, RvContext_t *context);

void handle_beq(uint32_t inst, RvContext_t *context);
void handle_bne(uint32_t inst, RvContext_t *context);
void handle_blt(uint32_t inst, RvContext_t *context);
void handle_bge(uint32_t inst, RvContext_t *context);
void handle_bltu(uint32_t inst, RvContext_t *context);
void handle_bgeu(uint32_t inst, RvContext_t *context);

void handle_lb(uint32_t inst, RvContext_t *context);
void handle_lh(uint32_t inst, RvContext_t *context);
void handle_lw(uint32_t inst, RvContext_t *context);
void handle_lbu(uint32_t inst, RvContext_t *context);
void handle_lhu(uint32_t inst, RvContext_t *context);
void handle_sb(uint32_t inst, RvContext_t *context);
void handle_sh(uint32_t inst, RvContext_t *context);
void handle_sw(uint32_t inst, RvContext_t *context);

void handle_addi(uint32_t inst, RvContext_t *context);
void handle_slti(uint32_t inst, RvContext_t *context);
void handle_sltiu(uint32_t inst, RvContext_t *context);
void handle_xori(uint32_t inst, RvContext_t *context);
void handle_ori(uint32_t inst, RvContext_t *context);
void handle_andi(uint32_t inst, RvContext_t *context);
void handle_slli(uint32_t inst, RvContext_t *context);
void handle_srli(uint32_t inst, RvContext_t *context);
void handle_srai(uint32_t inst, RvContext_t *context);

void handle_add(uint32_t inst, RvContext_t *context);
void handle_sub(uint32_t inst, RvContext_t *context);
void handle_sll(uint32_t inst, RvContext_t *context);
void handle_slt(uint32_t inst, RvContext_t *context);
void handle_sltu(uint32_t inst, RvContext_t *context);
void handle_xor(uint32_t inst, RvContext_t *context);
void handle_srl(uint32_t inst, RvContext_t *context);
void handle_sra(uint32_t inst, RvContext_t *context);
void handle_or(uint32_t inst, RvContext_t *context);
void handle_and(uint32_t inst, RvContext_t *context);

void handle_fence(uint32_t inst, RvContext_t *context);
void handle_ecall(uint32_t inst, RvContext_t *context);
void handle_ebreak(uint32_t inst, RvContext_t *context);
void handle_illegal_instruction(uint32_t inst, RvContext_t *context);

#endif
