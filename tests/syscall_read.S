#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  li a0, 0
  la a1, input_byte
  li a2, 1
  li a7, 63
  ecall
  assert_eq a0, 1, 1

  la t0, input_byte
  lbu t1, 0(t0)
  assert_eq t1, 'R', 2
  pass

.Lexit:
  li a7, 93
  ecall

.section .bss
input_byte:
  .space 1
