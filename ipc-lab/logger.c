#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    printf("[logger] PID %d starting, opening FIFO for reading...\n", getpid());

    int fifo_fd = open("events.fifo", O_RDONLY);
    if (fifo_fd == -1) {
        perror("open fifo failed");
        return 1;
    }
    printf("[logger] FIFO opened, fd = %d\n", fifo_fd);

    int log_fd = open("audit.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("open audit.log failed");
        return 1;
    }
    printf("[logger] audit.log opened, fd = %d\n", log_fd);

    char buf[256];
    ssize_t n;
    while ((n = read(fifo_fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("[logger] received: %s", buf);
        write(log_fd, buf, n);
    }

    printf("[logger] FIFO closed by all writers, shutting down.\n");
    close(fifo_fd);
    close(log_fd);
    return 0;
}
