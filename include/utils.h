#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdint.h>

uint8_t get_opcode(uint32_t inst);
uint8_t get_funct3(uint32_t inst);
uint8_t get_funct7(uint32_t inst);
uint8_t get_rd(uint32_t inst);
uint8_t get_rs1(uint32_t inst);
uint8_t get_rs2(uint32_t inst);

int32_t sign_extend(uint32_t value, int num_bits);
int32_t get_imm_i(uint32_t inst);
int32_t get_imm_s(uint32_t inst);
int32_t get_imm_b(uint32_t inst);
uint32_t get_imm_u(uint32_t inst);
int32_t get_imm_j(uint32_t inst);

uint32_t build_r_type(uint32_t opcode, uint32_t rd, uint32_t funct3,
                      uint32_t rs1, uint32_t rs2, uint32_t funct7);
uint32_t build_i_type(uint32_t opcode, uint32_t rd, uint32_t funct3,
                      uint32_t rs1, int32_t imm);
uint32_t build_s_type(uint32_t opcode, uint32_t funct3, uint32_t rs1,
                      uint32_t rs2, int32_t imm);
uint32_t build_b_type(uint32_t opcode, uint32_t funct3, uint32_t rs1,
                      uint32_t rs2, int32_t imm);
uint32_t build_u_type(uint32_t opcode, uint32_t rd, int32_t imm);
uint32_t build_j_type(uint32_t opcode, uint32_t rd, int32_t imm);

bool is_compressed(uint16_t half);

#endif
