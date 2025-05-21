#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool find_bss_range(unsigned long *start, unsigned long *end) {
  FILE *maps = fopen("/proc/self/maps", "r");
  if (!maps) {
    perror("fopen");
    return false;
  }

  char comm[256];
  FILE *comm_file = fopen("/proc/self/comm", "r");
  if (!comm_file) {
    perror("fopen");
    fclose(maps);
    return false;
  }

  if (!fgets(comm, sizeof(comm), comm_file)) {
    perror("fgets");
    fclose(comm_file);
    fclose(maps);
    return false;
  }

  fclose(comm_file);

  comm[strcspn(comm, "\n")] = '\0';

  char line[256];
  int found_executable = 0;

  while (fgets(line, sizeof(line), maps)) {
    // Line corresponds to the ELF file mapping?
    if (strstr(line, comm)) {
      found_executable = 1;
      continue;
    }

    // First anonymous mapping after the ELF is the BSS
    if (found_executable && strchr(line, '/') == NULL) {
      sscanf(line, "%lx-%lx", start, end);
      break;
    }
  }

  fclose(maps);

  return found_executable;
}