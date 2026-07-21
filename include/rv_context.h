#ifndef RV_CONTEXT_H
#define RV_CONTEXT_H

#include "cpu.h"
#include "memory.h"

typedef struct RvContext {
  CPU_t *cpu;
  Memory_t *memory;
} RvContext_t;

#endif
