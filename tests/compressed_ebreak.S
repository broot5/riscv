.option rvc
.section .text
.globl _start

_start:
  # EBREAK stops the emulator with a successful status.
  c.ebreak
