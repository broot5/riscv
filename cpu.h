#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_BASE_ADDR 0x00000000
#define PROGRAM_LOAD_VADDR 0x00000000
#define MEMORY_SIZE_BYTES 16 * 1024 * 1024 // 16MB

typedef struct CPU {
  uint32_t regs[32];
  uint32_t pc;
  uint8_t *memory;
  size_t mem_size;
  bool halt;
} CPU_t;

static inline void init_cpu(CPU_t *cpu) {
  memset(cpu->regs, 0, sizeof(cpu->regs));
  cpu->pc = PROGRAM_LOAD_VADDR;
  cpu->halt = false;

  cpu->memory = (uint8_t *)malloc(MEMORY_SIZE_BYTES);
  memset(cpu->memory, 0, MEMORY_SIZE_BYTES);
  cpu->mem_size = MEMORY_SIZE_BYTES;

  cpu->regs[2] = MEMORY_BASE_ADDR + MEMORY_SIZE_BYTES;
  return;
}

static inline void free_cpu(CPU_t *cpu) {
  free(cpu->memory);
  cpu->memory = NULL;

  return;
}

static inline uint32_t read_reg(CPU_t *cpu, unsigned int idx) {
  if (idx >= 32) {
    fprintf(stderr, "Invalid register read access: x%u\n", idx);
    cpu->halt = true;
    return 0;
  } else if (idx == 0)
    return 0;
  else
    return cpu->regs[idx];
}

static inline void write_reg(CPU_t *cpu, unsigned int idx, uint32_t value) {
  if (idx >= 32) {
    fprintf(stderr, "Invalid register write access: x%u\n", idx);
    cpu->halt = true;
    return;
  } else if (idx != 0)
    cpu->regs[idx] = value;

  return;
}

static inline uint8_t read_byte(CPU_t *cpu, uint32_t addr) {
  size_t index = addr - MEMORY_BASE_ADDR;

  if (addr < MEMORY_BASE_ADDR || index >= cpu->mem_size) {
    fprintf(stderr, "Memory access out of bounds (byte): 0x%08x\n", addr);
    cpu->halt = true;
    return 0;
  }
  return cpu->memory[index];
}

static inline uint16_t read_half(CPU_t *cpu, uint32_t addr) {
  size_t index = addr - MEMORY_BASE_ADDR;

  if (addr % sizeof(uint16_t) != 0) {
    perror("Unaligned half-word read");
    cpu->halt = true;
    return 0;
  }

  if (addr < MEMORY_BASE_ADDR || index >= cpu->mem_size - 1) {
    fprintf(stderr, "Memory access out of bounds (half): 0x%08x\n", addr);
    cpu->halt = true;
    return 0;
  }

  return (uint16_t)cpu->memory[index] | ((uint16_t)cpu->memory[index + 1] << 8);
}

static inline uint32_t read_word(CPU_t *cpu, uint32_t addr) {
  size_t index = addr - MEMORY_BASE_ADDR;

  if (addr % sizeof(uint32_t) != 0) {
    perror("Unaligned word read");
    cpu->halt = true;
    return 0;
  }

  if (addr < MEMORY_BASE_ADDR || index >= cpu->mem_size - 3) {
    fprintf(stderr, "Memory access out of bounds (word): 0x%08x\n", addr);
    cpu->halt = true;
    return 0;
  }

  return (uint32_t)cpu->memory[index] |
         ((uint32_t)cpu->memory[index + 1] << 8) |
         ((uint32_t)cpu->memory[index + 2] << 16) |
         ((uint32_t)cpu->memory[index + 3] << 24);
}

static inline void write_byte(CPU_t *cpu, uint32_t addr, uint8_t value) {
  size_t index = addr - MEMORY_BASE_ADDR;

  if (addr < MEMORY_BASE_ADDR || index >= cpu->mem_size) {
    fprintf(stderr, "Memory access out of bounds (byte): 0x%08x\n", addr);
    cpu->halt = true;
    return;
  }

  cpu->memory[index] = value;
  return;
}

static inline void write_half(CPU_t *cpu, uint32_t addr, uint16_t value) {
  size_t index = addr - MEMORY_BASE_ADDR;

  if (addr % sizeof(uint16_t) != 0) {
    perror("Unaligned half-word write");
    cpu->halt = true;
    return;
  }

  if (addr < MEMORY_BASE_ADDR || index >= cpu->mem_size - 1) {
    fprintf(stderr, "Memory access out of bounds (half): 0x%08x\n", addr);
    cpu->halt = true;
    return;
  }

  cpu->memory[index] = (uint8_t)(value & 0xFF);
  cpu->memory[index + 1] = (uint8_t)((value >> 8) & 0xFF);

  return;
}

static inline void write_word(CPU_t *cpu, uint32_t addr, uint32_t value) {
  size_t index = addr - MEMORY_BASE_ADDR;

  if (addr % sizeof(uint32_t) != 0) {
    perror("Unaligned word write");
    cpu->halt = true;
    return;
  }

  if (addr < MEMORY_BASE_ADDR || index >= cpu->mem_size - 3) {
    fprintf(stderr, "Memory access out of bounds (word): 0x%08x\n", addr);
    cpu->halt = true;
    return;
  }

  cpu->memory[index] = (uint8_t)(value & 0xFF);
  cpu->memory[index + 1] = (uint8_t)((value >> 8) & 0xFF);
  cpu->memory[index + 2] = (uint8_t)((value >> 16) & 0xFF);
  cpu->memory[index + 3] = (uint8_t)((value >> 24) & 0xFF);

  return;
}

static inline uint32_t fetch_instruction(CPU_t *cpu) {
  return read_word(cpu, cpu->pc);
}

#endif