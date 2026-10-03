#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000
#define BUF_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 2) { fprintf(stderr, "Usage: %s \"<command>\"\n", argv[0]); return 1; }

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) { perror("socket"); exit(1); }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    printf("[client] connecting to 127.0.0.1:%d ...\n", PORT);
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect"); exit(1);
    }
    printf("[client] connected, fd = %d\n", sock_fd);

    send(sock_fd, argv[1], strlen(argv[1]), 0);
    printf("[client] sent: \"%s\"\n", argv[1]);

    char buf[BUF_SIZE];
    ssize_t n = recv(sock_fd, buf, sizeof(buf) - 1, 0);
    if (n > 0) { buf[n] = '\0'; printf("[client] received: \"%s\"\n", buf); }

    close(sock_fd);
    return 0;
}
