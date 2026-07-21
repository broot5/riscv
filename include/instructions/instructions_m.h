#ifndef INSTRUCTIONS_M_H
#define INSTRUCTIONS_M_H

#include "rv_context.h"

#include <stdint.h>

void handle_mul(uint32_t inst, RvContext_t *context);
void handle_mulh(uint32_t inst, RvContext_t *context);
void handle_mulhsu(uint32_t inst, RvContext_t *context);
void handle_mulhu(uint32_t inst, RvContext_t *context);
void handle_div(uint32_t inst, RvContext_t *context);
void handle_divu(uint32_t inst, RvContext_t *context);
void handle_rem(uint32_t inst, RvContext_t *context);
void handle_remu(uint32_t inst, RvContext_t *context);

#endif
