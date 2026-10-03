#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define SHM_NAME "/sec2211_counter"
#define INCREMENTS 100000

int main() {
    // Create (or open) a shared memory object
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) { perror("shm_open"); return 1; }

    ftruncate(shm_fd, sizeof(int));

    int *counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                         MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) { perror("mmap"); return 1; }

    *counter = 0;
    printf("[main] shared counter created at %p, starting value = %d\n",
           (void *) counter, *counter);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1
        for (int i = 0; i < INCREMENTS; i++) {
            (*counter)++;
        }
        printf("[child1] done incrementing\n");
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2
        for (int i = 0; i < INCREMENTS; i++) {
            (*counter)++;
        }
        printf("[child2] done incrementing\n");
        exit(0);
    }

    // Parent waits for both children
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("[main] expected final value: %d\n", INCREMENTS * 2);
    printf("[main] actual final value:   %d\n", *counter);

    munmap(counter, sizeof(int));
    close(shm_fd);
    shm_unlink(SHM_NAME);
    return 0;
}
