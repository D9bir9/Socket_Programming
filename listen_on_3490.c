#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(){
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo; // points to result
    memset(&hints, 0, sizeof(hints)); // makes the struct is empty
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // fill in my ip for me

    if ((status = getaddrinfo(NULL, "3490", &hints, &servinfo)) != 0){
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        exit(1);
    }

    // servinfo now points to a linked list of one or more
    // addrinfos

    char address[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &servinfo, address, INET_ADDRSTRLEN);

    printf("%s\n", address);

    freeaddrinfo(servinfo); // free the linked list
    exit(0);
}
