#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "cpu.h"

static inline void handle_sys_read(CPU_t *cpu) {
  uint32_t fd = read_reg(cpu, 10);       // a0
  uint32_t buf_addr = read_reg(cpu, 11); // a1
  uint32_t count = read_reg(cpu, 12);    // a2

  if (buf_addr >= (MEMORY_BASE_ADDR + cpu->mem_size) ||
      count > (MEMORY_BASE_ADDR + cpu->mem_size) - buf_addr) {
    fprintf(stderr, "Error: ECALL read buffer out of bounds\n");
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  char *ptr = (char *)&cpu->memory[buf_addr - MEMORY_BASE_ADDR];
  ssize_t read_count = read(fd, ptr, count);
  write_reg(cpu, 10, (uint32_t)read_count);
}

static inline void handle_sys_write(CPU_t *cpu) {
  uint32_t fd = read_reg(cpu, 10);       // a0
  uint32_t buf_addr = read_reg(cpu, 11); // a1
  uint32_t count = read_reg(cpu, 12);    // a2

  if (buf_addr >= (MEMORY_BASE_ADDR + cpu->mem_size) ||
      count > (MEMORY_BASE_ADDR + cpu->mem_size) - buf_addr) {
    fprintf(stderr, "Error: ECALL write buffer out of bounds\n");
    cpu->exit_code = 1;
    cpu->halt = true;
    return;
  }

  char *ptr = (char *)&cpu->memory[buf_addr - MEMORY_BASE_ADDR];
  ssize_t written = write(fd, ptr, count);
  write_reg(cpu, 10, (uint32_t)written);
}

static inline void handle_sys_exit(CPU_t *cpu) {
  cpu->exit_code = read_reg(cpu, 10); // a0
  cpu->halt = true;
}

#endif
