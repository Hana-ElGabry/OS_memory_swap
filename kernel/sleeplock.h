#ifndef SLEEPLOCK_H// the line that i added 
#define SLEEPLOCK_H// the line that i added

// Long-term locks for processes
struct sleeplock {
  uint locked;       // Is the lock held?
  struct spinlock lk; // spinlock protecting this sleep lock
  char *name;        // Name of lock.
  int pid;           // Process holding lock
};

#endif // SLEEPLOCK_H // the line that i added

