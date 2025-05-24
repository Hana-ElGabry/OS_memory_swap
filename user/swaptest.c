#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MB (1024*1024)

int main() {
  printf("Starting swaptest...\n");
  
  // Allocate ~2MB (more than xv6's default physical memory)
  char *big = malloc(2 * MB);
  if(!big) {
    printf("Initial malloc failed!\n");
    exit(1);
  }

  printf("Filling memory...\n");
  // Write to each page to force allocation
  for(int i = 0; i < 2*MB; i += 4096) {
    big[i] = i % 256;
  }

  printf("Verifying...\n");
  // Verify values (this will trigger swapin if needed)
  for(int i = 0; i < 2*MB; i += 4096) {
    if(big[i] != (i % 256)) {
      printf("Mismatch at %d: %d\n", i, big[i]);
      exit(1);
    }
  }

  printf("swaptest completed successfully\n");
  exit(0);
}