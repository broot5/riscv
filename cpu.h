#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE_BYTES 16 * 1024 * 1024 // 16MB

typedef struct CPU {
  uint32_t regs[32];
  uint32_t pc;
  uint8_t *memory;
  size_t mem_size;
  uint32_t memory_base;
  int exit_code;
  bool halt;
} CPU_t;

static inline void init_cpu(CPU_t *cpu) {
  memset(cpu->regs, 0, sizeof(cpu->regs));
  cpu->pc = 0;
  cpu->exit_code = 0;
  cpu->halt = false;

  cpu->memory = (uint8_t *)calloc(1, MEMORY_SIZE_BYTES);
  if (!cpu->memory) {
    perror("Error: Failed to allocate memory");
    exit(EXIT_FAILURE);
  }
  cpu->mem_size = MEMORY_SIZE_BYTES;
  cpu->memory_base = 0;

  cpu->regs[2] = cpu->memory_base + cpu->mem_size;
  return;
}

static inline void free_cpu(CPU_t *cpu) {
  free(cpu->memory);
  cpu->memory = NULL;

  return;
}

static inline uint32_t read_reg(CPU_t *cpu, unsigned int idx) {
  if (idx >= 32) {
    fprintf(stderr, "Error: Invalid register read access: x%u\n", idx);
    cpu->exit_code = 1;
    cpu->halt = true;
    return 0;
  }
  if (idx == 0)
    return 0;
  return cpu->regs[idx];
}

static inline void write_reg(CPU_t *cpu, unsigned int idx, uint32_t value) {
  if (idx >= 32) {
    fprintf(stderr, "Error: Invalid register write access: x%u\n", idx);
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }
  if (idx == 0)
    return;
  cpu->regs[idx] = value;
}

static inline bool validate_mem_access(CPU_t *cpu, uint32_t addr, size_t size) {
  if (addr < cpu->memory_base ||
      addr - cpu->memory_base + size > cpu->mem_size) {
    fprintf(stderr,
            "Error: Memory access out of bounds (addr: 0x%08x, base: 0x%08x, "
            "size: %zu)\n",
            addr, cpu->memory_base, size);
    cpu->exit_code = 1;
    cpu->halt = true;
    return false;
  }
  return true;
}

static inline bool validate_alignment(CPU_t *cpu, uint32_t addr, size_t size) {
  if (addr % size != 0) {
    fprintf(stderr,
            "Error: Unaligned memory access (addr: 0x%08x, size: %zu)\n", addr,
            size);
    cpu->exit_code = 1;
    cpu->halt = true;
    return false;
  }
  return true;
}

static inline uint8_t read_byte(CPU_t *cpu, uint32_t addr) {
  if (!validate_mem_access(cpu, addr, 1))
    return 0;

  return cpu->memory[addr - cpu->memory_base];
}

static inline uint16_t read_half(CPU_t *cpu, uint32_t addr) {
  if (!validate_alignment(cpu, addr, 2) || !validate_mem_access(cpu, addr, 2))
    return 0;

  uint16_t val;
  memcpy(&val, &cpu->memory[addr - cpu->memory_base], 2);
  return val;
}

static inline uint32_t read_word(CPU_t *cpu, uint32_t addr) {
  if (!validate_alignment(cpu, addr, 4) || !validate_mem_access(cpu, addr, 4))
    return 0;

  uint32_t val;
  memcpy(&val, &cpu->memory[addr - cpu->memory_base], 4);
  return val;
}

static inline void write_byte(CPU_t *cpu, uint32_t addr, uint8_t value) {
  if (!validate_mem_access(cpu, addr, 1))
    return;

  cpu->memory[addr - cpu->memory_base] = value;
}

static inline void write_half(CPU_t *cpu, uint32_t addr, uint16_t value) {
  if (!validate_alignment(cpu, addr, 2) || !validate_mem_access(cpu, addr, 2))
    return;

  memcpy(&cpu->memory[addr - cpu->memory_base], &value, 2);
}

static inline void write_word(CPU_t *cpu, uint32_t addr, uint32_t value) {
  if (!validate_alignment(cpu, addr, 4) || !validate_mem_access(cpu, addr, 4))
    return;

  memcpy(&cpu->memory[addr - cpu->memory_base], &value, 4);
}

static inline uint32_t fetch_instruction(CPU_t *cpu) {
  return read_word(cpu, cpu->pc);
}

static inline void dump_registers(CPU_t *cpu) {
  fprintf(stderr,
          "================================================================\n");
  fprintf(stderr, "Register Dump:\n");
  fprintf(stderr, "PC : 0x%08x\n", cpu->pc);
  for (int i = 0; i < 32; i++) {
    fprintf(stderr, "x%-2d: 0x%08x ", i, cpu->regs[i]);
    if ((i + 1) % 4 == 0)
      fprintf(stderr, "\n");
  }
  fprintf(stderr,
          "================================================================\n");
}

#endif