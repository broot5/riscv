#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_SIZE_BYTES (16 * 1024 * 1024)

typedef struct Memory {
  uint8_t *data;
  size_t size;
  uint32_t base;
  uint32_t program_break;
} Memory_t;

bool init_memory(Memory_t *memory, size_t size);
void free_memory(Memory_t *memory);

bool validate_mem_access(const Memory_t *memory, uint32_t addr, size_t size);
bool validate_alignment(uint32_t addr, size_t size);
bool memory_get_pointer(Memory_t *memory, uint32_t addr, size_t size,
                        uint8_t **pointer);

bool read_byte(const Memory_t *memory, uint32_t addr, uint8_t *value);
bool read_half(const Memory_t *memory, uint32_t addr, uint16_t *value);
bool read_word(const Memory_t *memory, uint32_t addr, uint32_t *value);
bool write_byte(Memory_t *memory, uint32_t addr, uint8_t value);
bool write_half(Memory_t *memory, uint32_t addr, uint16_t value);
bool write_word(Memory_t *memory, uint32_t addr, uint32_t value);

#endif
