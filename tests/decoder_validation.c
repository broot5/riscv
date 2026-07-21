#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "decoder.h"
#include "memory.h"
#include "opcodes.h"
#include "rv_context.h"
#include "utils.h"

static bool test_valid_addi(void) {
  CPU_t cpu;
  init_cpu(&cpu);
  Memory_t memory;
  if (!init_memory(&memory, 64))
    return false;

  RvContext_t context = {.cpu = &cpu, .memory = &memory};
  write_reg(&cpu, 1, 5);
  decode_and_execute(build_i_type(OPCODE_OP_IMM, 2, 0b000, 1, -3), &context);

  bool passed = !cpu.halt && read_reg(&cpu, 2) == 2;
  free_memory(&memory);
  return passed;
}

static bool is_illegal(uint32_t inst) {
  CPU_t cpu;
  init_cpu(&cpu);
  Memory_t memory;
  if (!init_memory(&memory, 64))
    return false;

  RvContext_t context = {.cpu = &cpu, .memory = &memory};
  decode_and_execute(inst, &context);

  bool passed = cpu.halt && cpu.exit_code == 1;
  free_memory(&memory);
  return passed;
}

int main(void) {
  bool passed = test_valid_addi() && is_illegal(0) &&
                is_illegal(build_i_type(OPCODE_JALR, 1, 0b001, 0, 0)) &&
                is_illegal(build_b_type(OPCODE_BRANCH, 0b010, 0, 0, 0)) &&
                is_illegal(build_r_type(OPCODE_OP, 1, 0b000, 0, 0, 0b1111111));

  if (!passed) {
    fprintf(stderr, "FAIL  decoder_validation\n");
    return EXIT_FAILURE;
  }

  printf("PASS  decoder_validation\n");
  return EXIT_SUCCESS;
}
