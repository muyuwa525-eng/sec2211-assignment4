#include <stdio.h>
#include <arpa/inet.h>

int main() {
    unsigned short port_host = 9000;
    unsigned short port_net = htons(port_host);
    unsigned int val_host = 1;
    unsigned int val_net = htonl(val_host);

    printf("Host byte order port:    %u\n", port_host);
    printf("Network byte order port: %u (htons result)\n", port_net);
    printf("ntohs back to host:      %u\n", ntohs(port_net));
    printf("\n");
    printf("Host byte order 1:       0x%08x\n", val_host);
    printf("Network byte order 1:    0x%08x (htonl result)\n", val_net);
    printf("ntohl back to host:      0x%08x\n", ntohl(val_net));
    return 0;
}
