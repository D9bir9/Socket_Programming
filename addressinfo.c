#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(){
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo("www.google.com", "3490", &hints, &servinfo);
    if (status != 0){
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    struct addrinfo *info = servinfo;

    while (info != NULL){
        char ip[INET_ADDRSTRLEN];
        inet_ntop(info->ai_family, &info->ai_addr, ip, INET_ADDRSTRLEN);
        printf ("protocol: %d\n",info->ai_protocol);
        printf("Ip Address: %s\n", ip);
        info = info->ai_next;
    }

    freeaddrinfo(servinfo);
    exit(1);
}