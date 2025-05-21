#define _GNU_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "memfd-helpers.h"

bool map_memfd_to_bss(unsigned long bss_start, unsigned long bss_end,
                      unsigned long *offset) {
  // Calculate the size of the .bss section
  size_t bss_size = bss_end - bss_start;

  // Create a memfd
  int memfd = syscall(SYS_memfd_create, "bss_memfd", MFD_CLOEXEC);
  if (memfd == -1) {
    perror("memfd_create");
    return false;
  }

  // Resize the memfd to match the size of the .bss section
  if (ftruncate(memfd, bss_size) == -1) {
    perror("ftruncate");
    close(memfd);
    return false;
  }

  // Map the memfd over the .bss section
  void *mapped_ro = mmap((void *)bss_start, bss_size, PROT_READ,
                         MAP_SHARED | MAP_FIXED, memfd, 0);
  if (mapped_ro == MAP_FAILED) {
    perror("mmap");
    close(memfd);
    return false;
  }

  void *mapped_rw =
      mmap(NULL, bss_size, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, 0);
  if (mapped_rw == MAP_FAILED) {
    perror("mmap");
    close(memfd);
    return false;
  }

  // Close the memfd (the mapping remains valid)
  close(memfd);

  unsigned long bss_offset = (unsigned long)mapped_rw - bss_start;
  *offset = bss_offset;
  return true;
}