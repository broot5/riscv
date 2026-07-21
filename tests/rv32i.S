#include "include/test_macros.inc"

.option norvc
.section .text
.globl _start

_start:
  # OP-IMM
  addi t0, zero, -7
  assert_eq t0, -7, 1

  slti t1, t0, -6
  assert_eq t1, 1, 2

  sltiu t1, t0, 1
  assert_eq t1, 0, 3

  xori t1, t0, -1
  assert_eq t1, 6, 4

  ori t1, zero, 0x55
  assert_eq t1, 0x55, 5

  andi t1, t1, 0x0f
  assert_eq t1, 5, 6

  addi t0, zero, 1
  slli t1, t0, 5
  assert_eq t1, 32, 7

  addi t0, zero, -8
  srli t1, t0, 2
  assert_eq t1, 0x3ffffffe, 8

  srai t1, t0, 2
  assert_eq t1, -2, 9

  # Register-register operations
  li t0, 20
  li t1, 6
  add t2, t0, t1
  assert_eq t2, 26, 10

  sub t2, t0, t1
  assert_eq t2, 14, 11

  li t0, 3
  li t1, 4
  sll t2, t0, t1
  assert_eq t2, 48, 12

  li t0, -1
  li t1, 1
  slt t2, t0, t1
  assert_eq t2, 1, 13

  sltu t2, t0, t1
  assert_eq t2, 0, 14

  li t0, 0x55
  li t1, 0x0f
  xor t2, t0, t1
  assert_eq t2, 0x5a, 15

  or t2, t0, t1
  assert_eq t2, 0x5f, 16

  and t2, t0, t1
  assert_eq t2, 5, 17

  li t0, 0x80000000
  li t1, 4
  srl t2, t0, t1
  assert_eq t2, 0x08000000, 18

  sra t2, t0, t1
  assert_eq t2, 0xf8000000, 19

  # Upper immediates and PC-relative addressing
  lui t0, 0x12345
  assert_eq t0, 0x12345000, 20

.Lauipc_site:
  auipc t0, 0
  la t1, .Lauipc_site
  assert_regs_eq t0, t1, 21

  fence
  pass

.Lexit:
  li a7, 93
  ecall
