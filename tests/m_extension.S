#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  li t0, 7
  li t1, -3
  mul t2, t0, t1
  assert_eq t2, -21, 1

  li t0, -2
  li t1, 3
  mulh t2, t0, t1
  assert_eq t2, -1, 2
  mulhsu t2, t0, t1
  assert_eq t2, -1, 3

  li t0, -1
  li t1, 2
  mulhu t2, t0, t1
  assert_eq t2, 1, 4

  li t0, -7
  li t1, 3
  div t2, t0, t1
  assert_eq t2, -2, 5
  rem t2, t0, t1
  assert_eq t2, -1, 6

  li t0, 7
  divu t2, t0, t1
  assert_eq t2, 2, 7
  remu t2, t0, t1
  assert_eq t2, 1, 8

  # Division edge cases required by the M extension.
  div t2, t0, zero
  assert_eq t2, -1, 9
  rem t2, t0, zero
  assert_eq t2, 7, 10

  li t0, 0x80000000
  li t1, -1
  div t2, t0, t1
  assert_eq t2, 0x80000000, 11
  rem t2, t0, t1
  assert_eq t2, 0, 12
  pass

.Lexit:
  li a7, 93
  ecall
