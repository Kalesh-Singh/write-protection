#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "bss-helpers.h"
#include "memfd-helpers.h"
#include "range-map.h"
#include "signal-helpers.h"

// Initialize this (non-zero) to ensure it is NOT in .bss section
range_map_t *__attribute__((aligned(0x1000))) range_map =
    (range_map_t *)0x12345678;

// These will be placed in the .bss section
int __attribute__((aligned(0x1000))) global_x;

static bool setup_write_protect() {
  // Dynamically allocate the global map
  if (!init_range_map(&range_map, 10)) {
    fprintf(stderr, "Failed to initialize range map\n");
    return false;
  }

  unsigned long bss_start, bss_end;
  if (!find_bss_range(&bss_start, &bss_end)) {
    fprintf(stderr, "Failed to find BSS range\n");
    return false;
  }

  unsigned long offset;
  if (!map_memfd_to_bss(bss_start, bss_end, &offset)) {
    fprintf(stderr, "Failed to map memfd to BSS\n");
    return false;
  }

  range_t range = {bss_start, bss_end, offset};
  if (!add_range(range_map, range)) {
    fprintf(stderr, "Failed to add range to map\n");
    return false;
  }

  // Register the SIGSEGV handler
  if (!register_sigsegv_handler()) {
    fprintf(stderr, "Failed to register SIGSEGV handler\n");
    return false;
  }

  return true;
}

int main() {
  if (!setup_write_protect()) {
    fprintf(stderr, "Failed to setup write protection\n");
    return EXIT_FAILURE;
  }

  global_x = 0x12345678;
  printf("x: %x\n", global_x);

  return 0;
}