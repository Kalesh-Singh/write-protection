#ifndef _SIGNAL_HELPERS_H_
#define _SIGNAL_HELPERS_H_

#include <signal.h>
#include <stdbool.h>

#include "range-map.h"

extern int __attribute__((aligned(0x1000))) global_x;
extern range_map_t *__attribute__((aligned(0x1000))) range_map;

void handle_sigsegv(int sig, siginfo_t *info, void *context);

bool register_sigsegv_handler();

#endif /* _SIGNAL_HELPERS_H_ */