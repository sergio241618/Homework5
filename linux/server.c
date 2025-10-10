// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "caesar.h"

#define BUFSZ 1024

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <bind_ip> <port>\n", argv[0]);
        return 1;
    }
    const char* bind_ip = argv[1];
    int port = atoi(argv[2]);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    inet_pton(AF_INET, bind_ip, &addr.sin_addr);

    if (bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); return 1; }
    if (listen(sfd, 1) < 0) { perror("listen"); return 1; }
    printf("Server listening on %s:%d\n", bind_ip, port);

    struct sockaddr_in cli = {0};
    socklen_t clen = sizeof(cli);
    int cfd = accept(sfd, (struct sockaddr*)&cli, &clen);
    if (cfd < 0) { perror("accept"); return 1; }

    uint8_t buf[BUFSZ];
    ssize_t n = recv(cfd, buf, sizeof(buf), 0);
    if (n > 0) {
        char plain[BUFSZ];
        caesar_decrypt_bytes(buf, (size_t)n, plain, sizeof(plain));
        printf("Received %zd bytes. Decrypted: %s\n", n, plain);
        send(cfd, buf, (size_t)n, 0);
    }

    close(cfd);
    close(sfd);
    return 0;
}