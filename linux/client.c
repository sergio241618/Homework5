// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "caesar.h"

#define BUFSZ 1024

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <server_ip> <port> <message>\n", argv[0]);
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    const char* msg = argv[3];

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0) { perror("socket"); return 1; }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    if (connect(sfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }

    uint8_t pkt[BUFSZ];
    uint8_t shift = 5; // Elige tu desplazamiento (0-25)
    size_t n = caesar_encrypt_bytes(msg, shift, pkt, sizeof(pkt));
    send(sfd, pkt, n, 0);

    uint8_t reply[BUFSZ];
    ssize_t r = recv(sfd, reply, sizeof(reply), 0);
    if (r > 0) {
        char plain[BUFSZ];
        caesar_decrypt_bytes(reply, (size_t)r, plain, sizeof(plain));
        printf("Reply (%zd bytes). Decrypted: %s\n", r, plain);
    }

    close(sfd);
    return 0;
}