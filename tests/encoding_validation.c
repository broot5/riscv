#include "compressed_decoder.h"
#include "cpu.h"
#include "fetch.h"
#include "memory.h"
#include "opcodes.h"
#include "utils.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool test_sign_extension(void) {
  return sign_extend(UINT32_C(0x7ff), 12) == 2047 &&
         sign_extend(UINT32_C(0x800), 12) == -2048 &&
         sign_extend(UINT32_MAX, 32) == -1;
}

static bool test_instruction_builders(void) {
  uint32_t i_type = build_i_type(OPCODE_OP_IMM, 1, 0b000, 2, -1);
  uint32_t s_type = build_s_type(OPCODE_STORE, 0b010, 1, 2, -16);
  uint32_t b_type = build_b_type(OPCODE_BRANCH, 0b000, 1, 2, -4);
  uint32_t u_type = build_u_type(OPCODE_LUI, 1, (int32_t)UINT32_C(0xfffff000));
  uint32_t j_type = build_j_type(OPCODE_JAL, 1, -2);

  return i_type == UINT32_C(0xfff10093) && get_imm_i(i_type) == -1 &&
         get_imm_s(s_type) == -16 && get_imm_b(b_type) == -4 &&
         (b_type & UINT32_C(0x80000000)) != 0 &&
         get_imm_u(u_type) == UINT32_C(0xfffff000) && get_imm_j(j_type) == -2 &&
         (j_type & UINT32_C(0x80000000)) != 0;
}

static bool test_high_bit_fetch(void) {
  CPU_t cpu;
  init_cpu(&cpu);
  Memory_t memory;
  if (!init_memory(&memory, MEMORY_SIZE_BYTES))
    return false;

  bool wrote = write_word(&memory, 0, UINT32_C(0xfff10093));

  FetchResult_t fetch = fetch_instruction(&memory, cpu.pc);
  bool passed = wrote && fetch.success && fetch.len == 4 &&
                fetch.inst == UINT32_C(0xfff10093);

  free_memory(&memory);
  return passed;
}

static bool test_negative_compressed_immediates(void) {
  uint32_t addi = expand_compressed(UINT16_C(0x10fd));
  uint32_t addi16sp = expand_compressed(UINT16_C(0x717d));
  uint32_t lui = expand_compressed(UINT16_C(0x70fd));

  return get_opcode(addi) == OPCODE_OP_IMM && get_rd(addi) == 1 &&
         get_rs1(addi) == 1 && get_imm_i(addi) == -1 &&
         get_opcode(addi16sp) == OPCODE_OP_IMM && get_rd(addi16sp) == 2 &&
         get_rs1(addi16sp) == 2 && get_imm_i(addi16sp) == -16 &&
         get_opcode(lui) == OPCODE_LUI && get_rd(lui) == 1 &&
         get_imm_u(lui) == UINT32_C(0xfffff000);
}

int main(void) {
  bool passed = test_sign_extension() && test_instruction_builders() &&
                test_high_bit_fetch() && test_negative_compressed_immediates();

  if (!passed) {
    fprintf(stderr, "FAIL  encoding_validation\n");
    return EXIT_FAILURE;
  }

  printf("PASS  encoding_validation\n");
  return EXIT_SUCCESS;
}
