#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  li t0, 5
  li t1, 5
  beq t0, t1, .Lbeq_taken
  li a0, 1
  j .Lexit
.Lbeq_taken:
  bne t0, t1, .Lfail_2

  li t1, 6
  bne t0, t1, .Lbne_taken
  li a0, 3
  j .Lexit
.Lbne_taken:

  li t0, -2
  li t1, 1
  blt t0, t1, .Lblt_taken
  li a0, 4
  j .Lexit
.Lblt_taken:
  bge t0, t1, .Lfail_5

  bge t1, t0, .Lbge_taken
  li a0, 6
  j .Lexit
.Lbge_taken:

  # Unsigned, -2 is greater than 1.
  bltu t1, t0, .Lbltu_taken
  li a0, 7
  j .Lexit
.Lbltu_taken:
  bgeu t1, t0, .Lfail_8

  bgeu t0, t1, .Lbgeu_taken
  li a0, 9
  j .Lexit
.Lbgeu_taken:
  pass

.Lfail_2:
  li a0, 2
  j .Lexit
.Lfail_5:
  li a0, 5
  j .Lexit
.Lfail_8:
  li a0, 8

.Lexit:
  li a7, 93
  ecall
