#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>

#include "rv_context.h"

void decode_and_execute(uint32_t inst, RvContext_t *context);

#endif
