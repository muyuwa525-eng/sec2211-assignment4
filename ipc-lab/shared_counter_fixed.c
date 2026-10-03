#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SHM_NAME "/sec2211_counter_fixed"
#define INCREMENTS 100000

struct shared_data {
    int counter;
    sem_t sem;
};

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct shared_data));
    struct shared_data *data = mmap(NULL, sizeof(*data), PROT_READ | PROT_WRITE,
                                     MAP_SHARED, shm_fd, 0);

    data->counter = 0;
    sem_init(&data->sem, 1, 1); // shared between processes, starts "unlocked"

    pid_t pid1 = fork();
    if (pid1 == 0) {
        for (int i = 0; i < INCREMENTS; i++) {
            sem_wait(&data->sem);   // enter critical section
            data->counter++;
            sem_post(&data->sem);  // leave critical section
        }
        exit(0);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        for (int i = 0; i < INCREMENTS; i++) {
            sem_wait(&data->sem);
            data->counter++;
            sem_post(&data->sem);
        }
        exit(0);
    }

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("[main] expected final value: %d\n", INCREMENTS * 2);
    printf("[main] actual final value:   %d\n", data->counter);

    sem_destroy(&data->sem);
    munmap(data, sizeof(*data));
    close(shm_fd);
    shm_unlink(SHM_NAME);
    return 0;
}
