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

  // Fork a child process.
  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  }

  // In the child process, execute the shell.
  if (pid == 0) {
    execlp("/bin/bash", "bash", NULL);
    perror("execlp");
    exit(1);
  }

  // In the parent process, write the stdout and stderr of the shell to the shared memory segment.
  while (1) {
    char buf[1024];
    size_t n = read(0, buf, sizeof(buf));
    if (n == 0) {
      break;
    }
    write(1, buf, n);
    write(shm_id, buf, n);
  }

  // Wait for the child process to exit.
  wait(NULL);

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
