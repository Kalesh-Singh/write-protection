#define _GNU_SOURCE
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include "range-map.h"
#include "signal-helpers.h"

void handle_sigsegv(int sig, siginfo_t *info, void *context) {
  (void)sig; // Unused parameter

  unsigned long addr = (unsigned long)info->si_addr;
  ucontext_t *uc = (ucontext_t *)context;
  mcontext_t *mc = &uc->uc_mcontext;

  // Get the faulting instruction
  uint32_t *pc = (uint32_t *)(mc->pc);
  uint32_t instruction = *pc;

  unsigned int rn = (instruction >> 5) & 0x1F; // Base register (Rn)

  unsigned long offset = 0;
  if (!find_offset(range_map, addr, &offset)) {
    exit(EXIT_FAILURE);
  }

  // Naive approach: just add the offset to the base register (Rn)
  uc->uc_mcontext.regs[rn] += offset;
}

bool register_sigsegv_handler() {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = handle_sigsegv;
  sigemptyset(&sa.sa_mask);

  if (sigaction(SIGSEGV, &sa, NULL) == -1) {
    perror("sigaction");
    return false;
  }

  return true;
}
