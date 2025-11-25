#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "compressed_decoder.h"
#include "cpu.h"
#include "decoder.h"
#include "loader.h"
#include "utils.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <program.elf>\n", argv[0]);
    return EXIT_FAILURE;
  }

  CPU_t cpu;
  init_cpu(&cpu);

  InstructionHandler dispatch_table[128][8];
  init_dispatch_table(dispatch_table);

  load_elf(&cpu, argv[1]);

  if (cpu.halt) {
    free_cpu(&cpu);
    return EXIT_FAILURE;
  }

  while (!cpu.halt) {
    FetchResult_t fetch = fetch_instruction(&cpu);
    uint32_t inst = fetch.inst;
    int step = fetch.len;

    cpu.current_inst_len = fetch.len;

    if (step == 2) {
      inst = expand_compressed((uint16_t)inst);
    }

    cpu.next_pc = cpu.pc + step;

    dispatch_table[get_opcode(inst)][get_funct3(inst)](inst, &cpu);

    if (cpu.halt)
      break;

    cpu.pc = cpu.next_pc;
  }

  if (cpu.exit_code != 0) {
    dump_registers(&cpu);
  }

  free_cpu(&cpu);

  return cpu.exit_code;
}