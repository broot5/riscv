#include "include/test_macros.inc"

.option rvc
.section .text
.globl _start

_start:
  c.li s0, 5
  c.addi s0, 3
  c.slli s0, 1
  .option push
  .option norvc
  assert_eq s0, 16, 1
  .option pop

  c.srli s0, 1
  c.andi s0, 7
  .option push
  .option norvc
  assert_eq s0, 0, 2
  .option pop

  c.li s0, -8
  c.srai s0, 1
  .option push
  .option norvc
  assert_eq s0, -4, 3
  .option pop

  c.li t0, 12
  c.mv t1, t0
  c.add t1, t0
  .option push
  .option norvc
  assert_eq t1, 24, 4
  .option pop

  c.li s0, 7
  c.li s1, 3
  c.sub s0, s1
  .option push
  .option norvc
  assert_eq s0, 4, 5
  .option pop
  c.xor s0, s1
  .option push
  .option norvc
  assert_eq s0, 7, 6
  .option pop
  c.or s0, s1
  c.and s0, s1
  .option push
  .option norvc
  assert_eq s0, 3, 7
  .option pop

  c.addi16sp sp, -32
  c.addi4spn s0, sp, 16
  c.li s1, 21
  c.sw s1, 0(s0)
  c.lw a0, 0(s0)
  .option push
  .option norvc
  assert_eq a0, 21, 8
  .option pop

  c.li t0, 31
  c.swsp t0, 0(sp)
  c.lwsp t1, 0(sp)
  c.addi16sp sp, 32
  .option push
  .option norvc
  assert_eq t1, 31, 9
  .option pop

  c.li s0, 0
  c.beqz s0, .Lc_beqz_ok
  .option push
  .option norvc
  li a0, 10
  j .Lexit
  .option pop
.Lc_beqz_ok:
  c.li s0, 1
  c.bnez s0, .Lc_bnez_ok
  .option push
  .option norvc
  li a0, 11
  j .Lexit
  .option pop
.Lc_bnez_ok:

  c.j .Lc_j_ok
  .option push
  .option norvc
  li a0, 12
  j .Lexit
  .option pop
.Lc_j_ok:
  c.jal .Lc_jal_target
.Lafter_c_jal:
  .option push
  .option norvc
  assert_eq s0, 2, 14
  la t0, .Lc_jr_target
  .option pop
  c.jr t0
  .option push
  .option norvc
  li a0, 15
  j .Lexit
  .option pop

.Lc_jal_target:
  .option push
  .option norvc
  la t0, .Lafter_c_jal
  assert_regs_eq ra, t0, 13
  .option pop
  c.li s0, 2
  c.jr ra

.Lc_jr_target:
  .option push
  .option norvc
  la t0, .Lc_jalr_target
  .option pop
  c.jalr t0
.Lafter_c_jalr:
  .option push
  .option norvc
  assert_eq s0, 3, 17
  pass
  .option pop

.Lc_jalr_target:
  .option push
  .option norvc
  la t0, .Lafter_c_jalr
  assert_regs_eq ra, t0, 16
  .option pop
  c.li s0, 3
  c.jr ra

.Lexit:
  .option push
  .option norvc
  li a7, 93
  ecall
  .option pop
