#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  la t0, test_bytes
  lb t1, 0(t0)
  assert_eq t1, -128, 1
  lbu t1, 0(t0)
  assert_eq t1, 128, 2
  lb t1, 1(t0)
  assert_eq t1, 127, 3

  la t0, test_halves
  lh t1, 0(t0)
  assert_eq t1, 0xffff8001, 4
  lhu t1, 0(t0)
  assert_eq t1, 0x8001, 5

  la t0, test_word
  lw t1, 0(t0)
  assert_eq t1, 0x89abcdef, 6

  la t0, scratch
  li t1, 0xaabbccdd
  sw t1, 0(t0)
  lw t2, 0(t0)
  assert_regs_eq t1, t2, 7

  li t1, 0x55
  sb t1, 4(t0)
  lbu t2, 4(t0)
  assert_eq t2, 0x55, 8

  li t1, 0x6677
  sh t1, 6(t0)
  lhu t2, 6(t0)
  assert_eq t2, 0x6677, 9

  # The loader provides a writable 16 MiB stack above the image base.
  addi sp, sp, -16
  li t1, 0x13579bdf
  sw t1, 0(sp)
  lw t2, 0(sp)
  addi sp, sp, 16
  assert_regs_eq t1, t2, 10
  pass

.Lexit:
  li a7, 93
  ecall

.section .data
test_bytes:
  .byte 0x80, 0x7f
  .p2align 1
test_halves:
  .half 0x8001
  .p2align 2
test_word:
  .word 0x89abcdef

.section .bss
.p2align 2
scratch:
  .space 8
