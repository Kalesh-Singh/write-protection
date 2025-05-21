#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "range-map.h"

bool init_range_map(range_map_t **range_map, size_t capacity) {

  range_map_t *map = *range_map;
  map = malloc(sizeof(range_map_t));
  if (!map) {
    perror("malloc map");
    return false;
  }

  map->ranges = malloc(capacity * sizeof(range_t));
  if (!map->ranges) {
    perror("malloc ranges");
    return false;
  }

  map->size = 0;
  map->capacity = capacity;

  *range_map = map;
  return true;
}

bool add_range(range_map_t *range_map, range_t range) {
  if (range_map->size >= range_map->capacity) {
    fprintf(stderr, "Range map is full!\n");
    return false;
  }

  range_map->ranges[range_map->size] = range;
  range_map->size++;
  return true;
}

bool find_offset(range_map_t *range_map, unsigned long addr,
                 unsigned long *offset) {
  for (size_t i = 0; i < range_map->size; i++) {
    unsigned long start = range_map->ranges[i].start;
    unsigned long end = range_map->ranges[i].end;

    if (addr >= start && addr < end) {
      *offset = range_map->ranges[i].offset;
      return true;
    }
  }

  return false;
}