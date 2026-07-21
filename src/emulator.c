#include "emulator.h"

#include "compressed_decoder.h"
#include "utils.h"

RvStepResult rv_step(CPU_t *cpu, InstructionHandler dispatch_table[128][8]) {
  RvStepResult result = {.status = RV_STEP_STOPPED,
                         .pc = cpu->pc,
                         .raw_instruction = 0,
                         .decoded_instruction = 0,
                         .instruction_length = 0};

  if (cpu->halt)
    return result;

  FetchResult_t fetch = fetch_instruction(cpu);

  result.raw_instruction = fetch.inst;
  result.instruction_length = (uint8_t)fetch.len;

  if (cpu->halt)
    return result;

  uint32_t instruction = fetch.inst;

  if (fetch.len == 2)
    instruction = expand_compressed((uint16_t)fetch.inst);

  result.decoded_instruction = instruction;

  cpu->current_inst_len = (uint32_t)fetch.len;
  cpu->next_pc = cpu->pc + (uint32_t)fetch.len;

  uint8_t opcode = get_opcode(instruction);
  uint8_t funct3 = get_funct3(instruction);

  dispatch_table[opcode][funct3](instruction, cpu);

  if (cpu->halt)
    return result;

  cpu->pc = cpu->next_pc;
  result.status = RV_STEP_EXECUTED;

  return result;
}
