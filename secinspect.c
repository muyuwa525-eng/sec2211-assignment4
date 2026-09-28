#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

        printf("Opened %s, got fd %d\n", argv[1], fd);

    struct stat st;
    fstat(fd, &st);
    printf("File size: %lld bytes\n", (long long) st.st_size);

    char buf[64];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("Read %zd bytes: \"%s\"\n", n, buf);

    off_t pos = lseek(fd, 0, SEEK_CUR);
    printf("Offset after read: %lld\n", (long long) pos);

    lseek(fd, 0, SEEK_SET);
    printf("Offset after seeking back to start: %lld\n",
           (long long) lseek(fd, 0, SEEK_CUR));

    close(fd);
    return 0;
}
