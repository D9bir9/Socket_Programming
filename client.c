#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void sendall (int sockfd, const void *buf, size_t len);

int main(int argc, char *argv[]){

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname>\n", argv[0]);
        exit(1);
    }
    struct addrinfo hints, *res, *p;
    int servfd; // 
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    status = getaddrinfo(argv[1], "3490", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        servfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (servfd == -1) {
            perror("socket");
            continue;
        }

        if (connect(servfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(servfd);
            continue;
        }

        break; // Successfully connected
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect to server\n");
        exit(1);
    }

    char buffer[512];
    
    ssize_t bytes_received = recv(servfd, (void*)buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        perror("recv");
        exit(1);
    }
    else if (bytes_received == 0) {
        printf("Server closed the connection.\n");
        exit(0);
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received string
    printf("Received from server: %s\n", buffer);

    const char* msg = "Hello, server! Dabi was here.";
    sendall(servfd, msg, strlen(msg));

    close(servfd);
    freeaddrinfo(res);
    exit(0);
}

void sendall (int sockfd, const void *buf, size_t len){
    size_t total_sent = 0;
    while (total_sent < len){
        ssize_t sent = send(sockfd, (const char*)buf + total_sent, len - total_sent, 0);
        if (sent == -1){
            perror("send");
            exit(1);
        }
        total_sent += sent;
    }
}