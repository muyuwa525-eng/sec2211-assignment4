#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main() {
    printf("[producer-C] PID %d starting, opening FIFO for writing...\n", getpid());

    int fifo_fd = open("events.fifo", O_WRONLY);
    if (fifo_fd == -1) {
        perror("open fifo failed");
        return 1;
    }
    printf("[producer-C] FIFO opened, fd = %d\n", fifo_fd);

        const char *events[] = {
        "SECURITY_ALERT",
        "INVALID_REQUEST",
        "LOGIN_FAILED",
        "LOGIN_SUCCESS"
    };

    for (int i = 0; i < 4; i++) {
        char msg[128];
        time_t now = time(NULL);
        snprintf(msg, sizeof(msg), "[C] pid=%d event=%s time=%ld\n",
                 getpid(), events[i], (long) now);
        write(fifo_fd, msg, strlen(msg));
        printf("[producer-C] sent: %s", msg);
        sleep(1);
    }

    printf("[producer-C] done, closing fd\n");
    close(fifo_fd);
    return 0;
}
