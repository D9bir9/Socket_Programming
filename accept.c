#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MYPORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold

void sendall(int sockfd, const void *buf, size_t len);

int main(void){
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t addr_size;
    struct addrinfo hints, *res, *p;
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    status = getaddrinfo(NULL, MYPORT, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket");
            continue;
        }

        // Reuse the socket address to avoid "Address already in use" error
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }

        break; // Successfully bound
    }
    if (p == NULL) {
        fprintf(stderr, "Failed to bind socket\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    addr_size = sizeof their_addr;

    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd == -1) {
        perror("accept");
        exit(1);
    }

    char hostname[256];
    size_t hostname_len = sizeof(hostname);

    if (gethostname(hostname, hostname_len) == -1) {
        perror("gethostname");
        exit(1);
    }
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    if (getpeername(new_fd, (struct sockaddr *)&peer_addr, &peer_addr_len) == -1) {
        perror("getpeername");
        exit(1);
    }

    char peer_ip[INET6_ADDRSTRLEN]  ;
    if (peer_addr.ss_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)&peer_addr;
        inet_ntop(AF_INET, &ipv4->sin_addr, peer_ip, INET_ADDRSTRLEN);
    } 
    else if (peer_addr.ss_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&peer_addr;
        inet_ntop(AF_INET6, &ipv6->sin6_addr, peer_ip, INET6_ADDRSTRLEN);
    }
    else {
        fprintf(stderr, "Unknown address family\n");
        exit(1);
    }

    printf("Connected to client on host: %s\n", hostname);
    printf("Client IP address: %s\n", peer_ip);


    // ready to communicate on socket descriptor new_fd
    const char* msg = "Hello, client! Dabi was here.";
    size_t len =  strlen(msg);
    sendall(new_fd, msg, len);

    char buffer[512];
    ssize_t bytes_received = recv(new_fd, (void*)buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        perror("recv");
        exit(1);
    }
    else if (bytes_received == 0) {
        printf("Client closed the connection.\n");
        exit(0);
    }

    buffer[bytes_received] = '\0'; // Null-terminate the received string
    printf("Received from client: %s\n", buffer);

    close(new_fd);
    close(sockfd);
    freeaddrinfo(res);
    exit(0);
}

void sendall(int sockfd, const void *buf, size_t len){
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