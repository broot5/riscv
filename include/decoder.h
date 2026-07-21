#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>

#include "cpu.h"

typedef void (*InstructionHandler)(uint32_t, CPU_t *);

void init_dispatch_table(InstructionHandler dispatch_table[128][8]);

#endif
