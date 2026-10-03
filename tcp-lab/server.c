#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9000
#define BUF_SIZE 256

int main() {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) { perror("socket"); exit(1); }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);   // host-to-network short

    if (bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind"); exit(1);
    }
    if (listen(listen_fd, 5) == -1) { perror("listen"); exit(1); }
    printf("[server] listening on port %d, listening fd = %d, PID = %d\n",
           PORT, listen_fd, getpid());

    while (1) {
        printf("[server] calling accept() - blocking until a client connects...\n");
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd == -1) { perror("accept"); continue; }

        printf("[server] accepted: NEW fd = %d (listening fd %d unchanged), client port = %d\n",
               client_fd, listen_fd, ntohs(client_addr.sin_port));

        char buf[BUF_SIZE];
        ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) { close(client_fd); continue; }
        buf[n] = '\0';
        buf[strcspn(buf, "\r\n")] = 0;
        printf("[server] received: \"%s\" (%zd bytes)\n", buf, n);

        char response[BUF_SIZE];
        if (n > 100) {
            snprintf(response, sizeof(response), "ERROR request too long\n");
        } else if (strncmp(buf, "STATUS", 6) == 0) {
            snprintf(response, sizeof(response), "OK monitoring-service-active uptime=42\n");
        } else if (strncmp(buf, "SUBMIT ", 7) == 0) {
            FILE *log = fopen("events_net.log", "a");
            if (log) { fprintf(log, "%s\n", buf + 7); fclose(log); }
            snprintf(response, sizeof(response), "OK event logged\n");
        } else {
            snprintf(response, sizeof(response), "ERROR unknown command\n");
        }

        send(client_fd, response, strlen(response), 0);
        printf("[server] sent: \"%s\"", response);
        close(client_fd);
        printf("[server] closed client fd %d\n\n", client_fd);
    }
    return 0;
}
