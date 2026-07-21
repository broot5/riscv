#ifndef SYSCALL_H
#define SYSCALL_H

#include "cpu.h"

void handle_sys_read(CPU_t *cpu);
void handle_sys_write(CPU_t *cpu);
void handle_sys_exit(CPU_t *cpu);
void handle_sys_brk(CPU_t *cpu);

#endif
