/*
** showip.c
**
** show IP addresses for a host given on the command line
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    if (argc != 2){
        fprintf(stderr, "usage: showip hostname\n");
        return 1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    printf("IP addresses for: %s\n", argv[1]);

    for (p = res; p != NULL; p = p->ai_next){
        void *addr;
        char *ipver;

        struct sockaddr_in *ipv4;
        struct sockaddr_in6 *ipv6;

        if (p->ai_family == AF_INET){
            ipv4 = (struct sockaddr_in*) p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPV4";
        }
        if (p->ai_family == AF_INET6){
            ipv6 = (struct sockaddr_in6*) p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPV6";
        }

        // Convert the IP to string and print it
        inet_ntop(p->ai_family, &addr, ipstr, sizeof ipstr);
        printf("%s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(res);
    return 0;
}