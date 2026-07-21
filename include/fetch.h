#ifndef FETCH_H
#define FETCH_H

#include <stdbool.h>
#include <stdint.h>

#include "memory.h"

typedef struct FetchResult {
  uint32_t inst;
  int len;
  bool success;
} FetchResult_t;

FetchResult_t fetch_instruction(const Memory_t *memory, uint32_t pc);

#endif
