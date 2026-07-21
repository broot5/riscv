#include "fetch.h"

#include "utils.h"

FetchResult_t fetch_instruction(const Memory_t *memory, uint32_t pc) {
  uint16_t lower;
  if (!read_half(memory, pc, &lower))
    return (FetchResult_t){.success = false};

  if (!is_compressed(lower)) {
    uint16_t upper;
    if (!read_half(memory, pc + 2, &upper))
      return (FetchResult_t){.success = false};

    return (FetchResult_t){
        .inst = (uint32_t)lower | ((uint32_t)upper << 16),
        .len = 4,
        .success = true,
    };
  }

  return (FetchResult_t){.inst = lower, .len = 2, .success = true};
}
