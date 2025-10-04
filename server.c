#include "addrinfo.h"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

int tcp_listen(const char *hostname, const char *port){
    struct addrinfo *res = addrinfo("localhost", "1234");
    int retval = -1;
    if(res == NULL){
        goto err;
    }
    print_single_addrinfo(res);

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0){
        goto err;
    }
    fprintf(stderr, "sockfd=%d\n", sockfd);

    int err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if(err){
        perror("bind");
        goto err;
    }

    err = listen(sockfd, 10);
    if(err){
        perror("listen");
        goto err;
    }

    retval = sockfd;

err:
    freeaddrinfo(res);
    return retval;
}

int main(int argc, char **argv)
{
    const char * host = "localhost";
    const char * port = "1234";
    int sock_fd = tcp_listen(host, port);
    if(sock_fd < 0){
        return 1;
    }
    fprintf(stderr, "Listening on %s port %s\n", host, port);

    sleep(10);

    return 0;
}
