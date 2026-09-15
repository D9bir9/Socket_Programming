#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MYPORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold

int main(void){
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me

    getaddrinfo(NULL, MYPORT, &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);


    // Reuse the socket address to avoid "Address already in use" error
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    bind(sockfd, res->ai_addr, res->ai_addrlen);

    listen(sockfd, BACKLOG);

    addr_size = sizeof their_addr;

    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    // ready to communicate on socket descriptor new_fd


    exit(0);
}