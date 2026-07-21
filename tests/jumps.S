#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  jal ra, .Ljal_target
.Lafter_jal:
  assert_eq s0, 11, 2

  la t0, .Ljalr_target
  addi t0, t0, 1
  jalr s1, 0(t0)
.Lafter_jalr:
  assert_eq s0, 22, 4

  jal zero, .Ljal_zero_ok
  li a0, 5
  j .Lexit
.Ljal_zero_ok:
  pass

.Ljal_target:
  la t0, .Lafter_jal
  assert_regs_eq ra, t0, 1
  li s0, 11
  jalr zero, 0(ra)

.Ljalr_target:
  la t0, .Lafter_jalr
  assert_regs_eq s1, t0, 3
  li s0, 22
  jalr zero, 0(s1)

.Lexit:
  li a7, 93
  ecall
