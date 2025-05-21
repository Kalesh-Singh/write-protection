#ifndef _MEMFD_HELPERS_H_
#define _MEMFD_HELPERS_H_

#include <stdbool.h>

bool map_memfd_to_bss(unsigned long bss_start, unsigned long bss_end,
                      unsigned long *offset);

#endif /* _MEMFD_HELPERS_H_ */