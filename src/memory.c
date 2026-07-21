#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool contains_range(const Memory_t *memory, uint32_t addr, size_t size) {
  if (addr < memory->base)
    return false;

  uint32_t offset = addr - memory->base;
  return offset <= memory->size && size <= memory->size - offset;
}

bool init_memory(Memory_t *memory, size_t size) {
  memory->data = (uint8_t *)calloc(1, size);
  if (!memory->data) {
    perror("Error: Failed to allocate memory");
    memory->size = 0;
    memory->base = 0;
    memory->program_break = 0;
    return false;
  }

  memory->size = size;
  memory->base = 0;
  memory->program_break = 0;
  return true;
}

void free_memory(Memory_t *memory) {
  free(memory->data);
  memory->data = NULL;
  memory->size = 0;
  memory->base = 0;
  memory->program_break = 0;
}

bool validate_mem_access(const Memory_t *memory, uint32_t addr, size_t size) {
  if (contains_range(memory, addr, size))
    return true;

  fprintf(stderr,
          "Error: Memory access out of bounds (addr: 0x%08x, base: 0x%08x, "
          "size: %zu)\n",
          addr, memory->base, size);
  return false;
}

bool validate_alignment(uint32_t addr, size_t size) {
  if (addr % size != 0) {
    fprintf(stderr,
            "Error: Unaligned memory access (addr: 0x%08x, size: %zu)\n", addr,
            size);
    return false;
  }
  return true;
}

bool memory_get_pointer(Memory_t *memory, uint32_t addr, size_t size,
                        uint8_t **pointer) {
  if (!contains_range(memory, addr, size))
    return false;

  *pointer = &memory->data[addr - memory->base];
  return true;
}

bool read_byte(const Memory_t *memory, uint32_t addr, uint8_t *value) {
  if (!validate_mem_access(memory, addr, 1))
    return false;

  *value = memory->data[addr - memory->base];
  return true;
}

bool read_half(const Memory_t *memory, uint32_t addr, uint16_t *value) {
  if (!validate_alignment(addr, 2) || !validate_mem_access(memory, addr, 2))
    return false;

  memcpy(value, &memory->data[addr - memory->base], 2);
  return true;
}

bool read_word(const Memory_t *memory, uint32_t addr, uint32_t *value) {
  if (!validate_alignment(addr, 4) || !validate_mem_access(memory, addr, 4))
    return false;

  memcpy(value, &memory->data[addr - memory->base], 4);
  return true;
}

bool write_byte(Memory_t *memory, uint32_t addr, uint8_t value) {
  if (!validate_mem_access(memory, addr, 1))
    return false;

  memory->data[addr - memory->base] = value;
  return true;
}

bool write_half(Memory_t *memory, uint32_t addr, uint16_t value) {
  if (!validate_alignment(addr, 2) || !validate_mem_access(memory, addr, 2))
    return false;

  memcpy(&memory->data[addr - memory->base], &value, 2);
  return true;
}

bool write_word(Memory_t *memory, uint32_t addr, uint32_t value) {
  if (!validate_alignment(addr, 4) || !validate_mem_access(memory, addr, 4))
    return false;

  memcpy(&memory->data[addr - memory->base], &value, 4);
  return true;
}
