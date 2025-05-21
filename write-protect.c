#define _GNU_SOURCE
#include <getopt.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bss-helpers.h"
#include "memfd-helpers.h"
#include "range-map.h"
#include "signal-helpers.h"

// Initialize this (non-zero) to ensure it is NOT in .bss section
range_map_t *__attribute__((aligned(0x1000))) range_map =
    (range_map_t *)0x12345678;

// These will be placed in the .bss section
int __attribute__((aligned(0x1000))) global_x;

typedef struct {
  int enable_write_protect;
  int iterations;
} program_options_t;

void print_help(const char *progname) {
  printf("Usage: %s [OPTIONS]\n", progname);
  printf("  --write_protect        Enable write protection\n");
  printf("  --iterations N         Number of iterations (default: 1)\n");
  printf("  --help                 Show this help message\n");
}

void parse_args(int argc, char **argv, program_options_t *opts) {
  static struct option long_options[] = {
      {"write_protect", no_argument, 0, 'w'},
      {"iterations", required_argument, 0, 'i'},
      {"help", no_argument, 0, 'h'},
      {0, 0, 0, 0}};

  opts->enable_write_protect = 0;
  opts->iterations = 1;

  int opt;
  while ((opt = getopt_long(argc, argv, "wi:h", long_options, NULL)) != -1) {
    switch (opt) {
    case 'w':
      opts->enable_write_protect = 1;
      break;
    case 'i':
      opts->iterations = atoi(optarg);
      if (opts->iterations < 1)
        opts->iterations = 1;
      break;
    case 'h':
      print_help(argv[0]);
      exit(EXIT_SUCCESS);
    default:
      print_help(argv[0]);
      exit(EXIT_FAILURE);
    }
  }
}

static inline uint64_t timespec_diff_ns(const struct timespec *start,
                                        const struct timespec *end) {
  return (end->tv_sec - start->tv_sec) * 1000000000ULL +
         (end->tv_nsec - start->tv_nsec);
}

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

int main(int argc, char **argv) {
  program_options_t opts;
  parse_args(argc, argv, &opts);

  if (opts.enable_write_protect) {
    printf("Write protection enabled\n");
    if (!setup_write_protect()) {
      fprintf(stderr, "Failed to setup write protection\n");
      return EXIT_FAILURE;
    }
  } else {
    printf("Write protection disabled\n");
  }

  srand((unsigned int)time(NULL));

  int n = opts.iterations;
  double times_ns;
  double sum = 0.0;
  double sum_sq = 0.0;

  for (int j = 0; j < n; ++j) {
    int r = rand();
    struct timespec t1, t2;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    global_x += r;
    clock_gettime(CLOCK_MONOTONIC, &t2);

    times_ns = (double)timespec_diff_ns(&t1, &t2);
    sum += times_ns;
    sum_sq += times_ns * times_ns;
  }

  // Write performance
  double avg = sum / n;
  double stddev = sqrt(sum_sq / n - avg * avg);

  printf("Iterations: %d\n", n);
  printf("Average write time: %.2f ns\n", avg);
  printf("Standard deviation: %.2f ns\n", stddev);

  return 0;
}