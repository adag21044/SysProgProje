#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>//Windows üzerinde açılmıyor


int main() {
  // Create shared memory.
  int shmid = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
  if (shmid == -1) {
    perror("shm_open");
    exit(1);
  }

  // Map shared memory to the address space of the process.
  void *addr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // Write to shared memory.
  sprintf(addr, "This is a message from the single-shell application.\n");

  // Close the shared memory segment.
  shm_unlink("/my_shared_memory");

  // Return success.
  return 0;
}