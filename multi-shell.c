#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/types.h>

int main() {
  // Create a shared memory segment.
  int shm_id = shm_open("shared_memory", O_CREAT | O_RDWR, 0666);
  if (shm_id == -1) {
    perror("shm_open");
    exit(1);
  }

  // Map the shared memory segment into the process's address space.
  void *addr = mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  if (addr == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // Create a number of instances of the `single-shell` application.
  for (int i = 0; i < 10; i++) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(1);
    }
    if (pid == 0) {
      execl("single-shell", "single-shell", NULL);
      perror("execl");
      exit(1);
    }
  }

  // Wait for all of the `single-shell` applications to exit.
  while (1) {
    pid_t pid = wait(NULL);
    if (pid == -1) {
      if (errno == EINTR) {
        continue;
      } else {
        perror("wait");
        exit(1);
      }
    }
  }

  // Write the data in the shared memory segment to a log file.
  FILE *log_file = fopen("log.txt", "w");
  if (log_file == NULL) {
    perror("fopen");
    exit(1);
  }
  fwrite(addr, 1, 1024, log_file);
  fclose(log_file);

  // Detach the shared memory segment from the process's address space.
  munmap(addr, 1024);

  // Close the shared memory segment.
  shm_unlink("shared_memory");

  return 0;
}
