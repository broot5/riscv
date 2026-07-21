#include "syscall.h"

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

static bool get_syscall_buffer(Memory_t *memory, uint32_t buf_addr,
                               uint32_t count, const char *op, char **buffer) {
  uint8_t *pointer;
  if (!memory_get_pointer(memory, buf_addr, count, &pointer)) {
    fprintf(stderr, "Error: ECALL %s buffer out of bounds\n", op);
    return false;
  }

  *buffer = (char *)pointer;
  return true;
}

void handle_sys_read(CPU_t *cpu, Memory_t *memory) {
  uint32_t fd = read_reg(cpu, 10);
  uint32_t buf_addr = read_reg(cpu, 11);
  uint32_t count = read_reg(cpu, 12);

  char *buffer;
  if (!get_syscall_buffer(memory, buf_addr, count, "read", &buffer)) {
    write_reg(cpu, 10, (uint32_t)-1);
    return;
  }

  ssize_t read_count = read(fd, buffer, count);
  write_reg(cpu, 10, (uint32_t)read_count);
}

void handle_sys_write(CPU_t *cpu, Memory_t *memory) {
  uint32_t fd = read_reg(cpu, 10);
  uint32_t buf_addr = read_reg(cpu, 11);
  uint32_t count = read_reg(cpu, 12);

  char *buffer;
  if (!get_syscall_buffer(memory, buf_addr, count, "write", &buffer)) {
    write_reg(cpu, 10, (uint32_t)-1);
    return;
  }

  ssize_t written = write(fd, buffer, count);
  write_reg(cpu, 10, (uint32_t)written);
}

void handle_sys_exit(CPU_t *cpu) {
  cpu->exit_code = read_reg(cpu, 10);
  cpu->halt = true;
}

void handle_sys_brk(CPU_t *cpu, Memory_t *memory) {
  uint32_t new_brk = read_reg(cpu, 10);

  if (new_brk == 0) {
    write_reg(cpu, 10, memory->program_break);
    return;
  }

  if (new_brk < memory->base || new_brk - memory->base > memory->size) {
    write_reg(cpu, 10, memory->program_break);
    return;
  }

  uint32_t sp = read_reg(cpu, 2);
  if (new_brk >= sp - 4096) {
    write_reg(cpu, 10, memory->program_break);
    return;
  }

  memory->program_break = new_brk;
  write_reg(cpu, 10, new_brk);
}
