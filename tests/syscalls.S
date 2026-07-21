#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  # brk(0) returns the current program break.
  li a0, 0
  li a7, 214
  ecall
  mv s0, a0

  addi s1, s0, 256
  mv a0, s1
  li a7, 214
  ecall
  assert_regs_eq a0, s1, 1

  li a0, 0
  li a7, 214
  ecall
  assert_regs_eq a0, s1, 2

  # write(1, message, message_len)
  li a0, 1
  la a1, message
  li a2, message_end - message
  li a7, 64
  ecall
  assert_eq a0, message_end - message, 3
  pass

.Lexit:
  li a7, 93
  ecall

.section .rodata
message:
  .ascii "syscalls: OK\n"
message_end:
