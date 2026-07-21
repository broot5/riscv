.macro assert_eq register, expected, code
  li t6, \expected
  beq \register, t6, .Lassert_eq_ok\@
  li a0, \code
  j .Lexit
.Lassert_eq_ok\@:
.endm

.macro assert_regs_eq lhs, rhs, code
  beq \lhs, \rhs, .Lassert_regs_eq_ok\@
  li a0, \code
  j .Lexit
.Lassert_regs_eq_ok\@:
.endm

.macro pass
  li a0, 0
  j .Lexit
.endm
