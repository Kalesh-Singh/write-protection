#ifndef _RANGE_MAP_H_
#define _RANGE_MAP_H_

#include <stdbool.h>

typedef struct {
  unsigned long start;
  unsigned long end;
  unsigned long offset;
} range_t;

typedef struct {
  range_t *ranges;
  size_t size;
  size_t capacity;
} range_map_t;

bool init_range_map(range_map_t **range_map, size_t capacity);

bool add_range(range_map_t *range_map, range_t range);

bool find_offset(range_map_t *range_map, unsigned long addr,
                 unsigned long *offset);

#endif /* _RANGE_MAP_H_ */