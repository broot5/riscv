#ifndef EMULATOR_H
#define EMULATOR_H

#include "rv_context.h"

typedef enum { RV_STEP_EXECUTED, RV_STEP_STOPPED } RvStepStatus;

typedef struct {
  RvStepStatus status;
  uint32_t pc;
  uint32_t raw_instruction;
  uint32_t decoded_instruction;
  uint8_t instruction_length;
} RvStepResult;

RvStepResult rv_step(RvContext_t *context);

#endif
