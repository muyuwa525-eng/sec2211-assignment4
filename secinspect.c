/*
 * secinspect.c
 * SEC 2211 - Practical Assignment 2, Question 1
 *
 * Demonstrates: open(), read(), write(), lseek(), fstat(), close(), dup2()
 *
 * Usage:
 *   ./secinspect <filename>
 *
 * What it does, step by step (each step prints what it is doing):
 *   1. open() the file
 *   2. fstat() it and print metadata
 *   3. read() the first bytes and print them
 *   4. lseek() to the start and to the end, printing the offset each time
 *   5. write() a timestamped marker line into the file
 *   6. dup2() to redirect stdout to a file, print a message, then restore stdout
 *   7. attempt an operation on a bad/closed descriptor to show error handling
 *   8. close() everything properly
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define READ_BUF_SIZE 128

static void section(const char *title) {
    printf("\n==================== %s ====================\n", title);
}

/* Print permission bits like ls -l does (rwxrwxrwx) */
static void print_perms(mode_t mode) {
    char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';
    printf("Permissions: %s\n", perms);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int fd;
    ssize_t n;
    off_t pos;
    char buf[READ_BUF_SIZE];
    struct stat st;

    /* -------- Standard descriptors info (Task 1.2) -------- */
    section("STANDARD FILE DESCRIPTORS");
    printf("FD 0 = stdin, FD 1 = stdout, FD 2 = stderr (always open when the process starts)\n");
    printf("PID of this process: %d  (look it up in /proc/%d/fd)\n", getpid(), getpid());

    /* -------- open() -------- */
    section("OPEN");
    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open() failed");
        return 1;
    }
    printf("open(\"%s\", O_RDWR|O_CREAT) succeeded.\n", filename);
    printf("Returned file descriptor: %d\n", fd);
    printf("(Note: 0, 1, 2 were already taken, so this is the next free slot.)\n");

    /* -------- fstat() -------- */
    section("FSTAT - FILE METADATA");
    if (fstat(fd, &st) == -1) {
        perror("fstat() failed");
    } else {
        printf("Size:  %lld bytes\n", (long long) st.st_size);
        printf("Inode: %lu\n", (unsigned long) st.st_ino);
        print_perms(st.st_mode);
        printf("Owner UID: %d, GID: %d\n", st.st_uid, st.st_gid);
    }

    /* -------- read() -------- */
    section("READ");
    n = read(fd, buf, READ_BUF_SIZE - 1);
    if (n == -1) {
        perror("read() failed");
    } else {
        buf[n] = '\0';
        printf("read() returned %zd bytes.\n", n);
        printf("Content read: \"%s\"\n", buf);
    }

    /* -------- lseek() -------- */
    section("LSEEK");
    pos = lseek(fd, 0, SEEK_CUR);
    printf("Current offset after read: %lld\n", (long long) pos);

    pos = lseek(fd, 0, SEEK_SET);
    printf("lseek(fd, 0, SEEK_SET) -> offset now: %lld (back to start)\n", (long long) pos);

    pos = lseek(fd, 0, SEEK_END);
    printf("lseek(fd, 0, SEEK_END) -> offset now: %lld (end of file)\n", (long long) pos);

    /* -------- write() -------- */
    section("WRITE");
    time_t now = time(NULL);
    char marker[256];
    int mlen = snprintf(marker, sizeof(marker),
                         "[secinspect] wrote marker at %s", ctime(&now));
    n = write(fd, marker, mlen);
    if (n == -1) {
        perror("write() failed");
    } else {
        printf("write() succeeded, %zd bytes written to end of file.\n", n);
    }

    /* -------- dup2() redirection demo (Task 1.3) -------- */
    section("DUP2 REDIRECTION DEMO");
    int out_fd = open("secinspect_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out_fd == -1) {
        perror("open() for redirection target failed");
    } else {
        printf("Before dup2(): FD 1 (stdout) points to the terminal.\n");
        fflush(stdout);

        int saved_stdout = dup(1);          /* keep a copy so we can restore it */
        dup2(out_fd, 1);                    /* FD 1 now points to secinspect_output.txt */

        printf("This line was written using the SAME printf() call, ");
        printf("but it now lands in secinspect_output.txt, not the terminal.\n");
        fflush(stdout);

        dup2(saved_stdout, 1);              /* restore FD 1 back to the terminal */
        close(saved_stdout);
        close(out_fd);

        printf("After dup2() restore: FD 1 points back to the terminal.\n");
        printf("Check secinspect_output.txt to see the redirected line.\n");
    }

    /* -------- Error handling demo -------- */
    section("ERROR HANDLING ON A BAD DESCRIPTOR");
    int bad_fd = 9999;
    ssize_t bad = read(bad_fd, buf, 10);
    if (bad == -1) {
        printf("read() on invalid fd %d failed as expected.\n", bad_fd);
        printf("errno = %d (%s)\n", errno, strerror(errno));
    }

    /* -------- close() -------- */
    section("CLOSE");
    if (close(fd) == -1) {
        perror("close() failed");
    } else {
        printf("fd %d closed successfully.\n", fd);
    }

    /* Demonstrate closing an already-closed fd -> should fail */
    if (close(fd) == -1) {
        printf("Closing fd %d a second time correctly failed: %s\n", fd, strerror(errno));
    }

    section("DONE");
    return 0;
}
