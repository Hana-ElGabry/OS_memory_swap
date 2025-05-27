#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define KB (1024)
#define NCHUNKS 16
#define CHUNK_SIZE (64*KB)  // 64KB chunks
#define PGSIZE 4096  // Page size in xv6

int main() {
  printf("Starting swaptest...\n");
  
  // Allocate memory in smaller chunks
  char *chunks[NCHUNKS];  // 16 chunks of 64KB each = 1MB total
  int i;
  
  // First allocate all chunks
  for(i = 0; i < NCHUNKS; i++) {
    printf("Allocating chunk %d (64KB)...\n", i);
    chunks[i] = malloc(CHUNK_SIZE);
    if(!chunks[i]) {
      printf("malloc failed at chunk %d!\n", i);
      exit(1);
    }
  }

  // Write to chunks in a pattern that will exercise swapping
  printf("Writing to chunks...\n");
  for(i = 0; i < NCHUNKS; i++) {
    printf("Writing to chunk %d...\n", i);
    for(int j = 0; j < CHUNK_SIZE; j += PGSIZE) {
      chunks[i][j] = (i + j) & 0xFF;
      sleep(1);  // Give the system time to handle swapping
    }
  }

  // Verify in reverse order to force swapping
  printf("Verifying chunks...\n");
  for(i = NCHUNKS-1; i >= 0; i--) {
    printf("Verifying chunk %d...\n", i);
    for(int j = 0; j < CHUNK_SIZE; j += PGSIZE) {
      if(chunks[i][j] != ((i + j) & 0xFF)) {
        printf("Mismatch in chunk %d at offset %d: expected %d, got %d\n",
               i, j, (i + j) & 0xFF, chunks[i][j]);
        exit(1);
      }
      sleep(1);  // Give the system time to handle swapping
    }
  }

  printf("swaptest completed successfully\n");
  exit(0);
}