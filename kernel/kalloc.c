// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)(end + 2 * 1024 * 1024)); // Allocate 2MB physical memory
}



void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    kfree(p);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");


#ifndef LAB_SYSCALL
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);
#endif
  
  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}



// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.

void* kalloc() {
  struct run *r;
  acquire(&kmem.lock);
  r = kmem.freelist;
  if(!r) {
    release(&kmem.lock);
    printf("kalloc: attempting swapout...\n");
    struct proc *p = myproc();
    if(p == 0) {
      panic("kalloc: no process");
    }
    uint64 victim_va = select_victim(p);
    if(victim_va == 0) {
      printf("kalloc: no victim found\n");
      return 0;  // Return null instead of panic
    }
    if(swapout(p, victim_va) < 0) {
      printf("kalloc: swapout failed\n");
      return 0;  // Return null instead of panic
    }
    printf("Evicted va %p\n", (void*)victim_va);
    return kalloc();  // Retry allocation after swap
  }

  kmem.freelist = r->next;
  release(&kmem.lock);
  memset((char*)r, 5, PGSIZE);
  return (void*)r;
}

//==============================================================
