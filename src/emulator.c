#include "emulator.h"

#include "compressed_decoder.h"
#include "decoder.h"
#include "fetch.h"

RvStepResult rv_step(RvContext_t *context) {
  CPU_t *cpu = context->cpu;
  RvStepResult result = {.status = RV_STEP_STOPPED,
                         .pc = cpu->pc,
                         .raw_instruction = 0,
                         .decoded_instruction = 0,
                         .instruction_length = 0};

  if (cpu->halt)
    return result;

  FetchResult_t fetch = fetch_instruction(context->memory, cpu->pc);

  if (!fetch.success) {
    cpu->exit_code = 1;
    cpu->halt = true;
    return result;
  }

  result.raw_instruction = fetch.inst;
  result.instruction_length = (uint8_t)fetch.len;

  uint32_t instruction = fetch.inst;

  if (fetch.len == 2)
    instruction = expand_compressed((uint16_t)fetch.inst);

  result.decoded_instruction = instruction;

  cpu->current_inst_len = (uint32_t)fetch.len;
  cpu->next_pc = cpu->pc + (uint32_t)fetch.len;

  decode_and_execute(instruction, context);

  if (cpu->halt)
    return result;

  cpu->pc = cpu->next_pc;
  result.status = RV_STEP_EXECUTED;

  return result;
}
