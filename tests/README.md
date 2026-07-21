# RISC-V instruction tests

Run the complete suite from the repository root:

```sh
make test
```

The host emulator is written to `build/riscv`.

Each assembly source builds into a freestanding, static RV32 ELF under
`build/tests/`. Tests exit with status `0` on success. A non-zero status is the
number of the failed assertion in that source file, so the source identifies
the exact operation that failed.

The suite covers:

- RV32I immediate, register, upper-immediate, and fence instructions
- signed and unsigned branches
- `JAL` and `JALR`, including link-register behavior
- byte, halfword, and word loads and stores
- RV32M multiplication, division, remainder, and edge cases
- supported RV32C integer instructions, jumps, branches, stack operations,
  and compressed `EBREAK`
- `read`, `write`, `exit`, and `brk` system calls

To inspect a generated test binary:

```sh
llvm-readelf -h build/tests/rv32i.elf
llvm-objdump -d build/tests/rv32i.elf
```
