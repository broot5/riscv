#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEMORY_SIZE_BYTES (16 * 1024 * 1024)

typedef struct CPU {
  uint32_t regs[32];
  uint32_t pc;
  uint32_t next_pc;
  uint32_t current_inst_len;
  uint8_t *memory;
  size_t mem_size;
  uint32_t memory_base;
  uint32_t program_break;
  int exit_code;
  bool halt;
} CPU_t;

typedef struct FetchResult {
  uint32_t inst;
  int len;
} FetchResult_t;

void init_cpu(CPU_t *cpu);
void free_cpu(CPU_t *cpu);

uint32_t read_reg(CPU_t *cpu, unsigned int idx);
void write_reg(CPU_t *cpu, unsigned int idx, uint32_t value);

bool validate_mem_access(CPU_t *cpu, uint32_t addr, size_t size);
bool validate_alignment(CPU_t *cpu, uint32_t addr, size_t size);
uint8_t read_byte(CPU_t *cpu, uint32_t addr);
uint16_t read_half(CPU_t *cpu, uint32_t addr);
uint32_t read_word(CPU_t *cpu, uint32_t addr);
void write_byte(CPU_t *cpu, uint32_t addr, uint8_t value);
void write_half(CPU_t *cpu, uint32_t addr, uint16_t value);
void write_word(CPU_t *cpu, uint32_t addr, uint32_t value);

FetchResult_t fetch_instruction(CPU_t *cpu);
void dump_registers(CPU_t *cpu);

#endif
