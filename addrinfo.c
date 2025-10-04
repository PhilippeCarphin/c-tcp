#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> // Struct addrinfo

#include <arpa/inet.h>
#include <string.h> // memset
#include <errno.h>
#include <stdio.h>

#include "addrinfo.h"

int print_single_addrinfo(struct addrinfo *a){
    char ipstr[INET6_ADDRSTRLEN];
    void *addr;
    char *type;
    if(a->ai_family == AF_INET){
        struct sockaddr_in *ipv4 = (struct sockaddr_in*) a->ai_addr;
        addr = &(ipv4->sin_addr);
        type = "IPV4";
    } else {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*) a->ai_addr;
        addr = &(ipv6->sin6_addr);
        type = "IPV6";
    }
    inet_ntop(a->ai_family, addr, ipstr, sizeof(ipstr));
    fprintf(stderr, "addrinfo: a->ai_family=%d, (%s)\n", a->ai_family, type);
    fprintf(stderr, "addrinfo: a->ai_protocol=%d\n", a->ai_protocol);
    fprintf(stderr, "addrinfo: a->ai_family=%s\n", a->ai_canonname);
    fprintf(stderr, "%s: %s\n", type, ipstr);
    return 0;
}
int print_addrinfo(struct addrinfo *head){
    for(struct addrinfo *cur = head; cur != NULL; cur = cur->ai_next){
        print_single_addrinfo(cur);
    }
    return 0;
}

struct addrinfo *addrinfo(const char *hostname, const char *port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME | AI_V4MAPPED_CFG | AI_PASSIVE;
    struct addrinfo *res;
    int err = getaddrinfo(hostname, port, &hints, &res);
    if(err){
        perror("getaddrinfo");
        return NULL;
    }

    return res;
}

int seeaddrinfo(const char *hostname, const char *port){
    struct addrinfo *res = addrinfo("google.com", "80");
    if(res == NULL){
        fprintf(stderr, "No results for address %s, port %s\n", hostname, port);
        return 1;
    }
    print_addrinfo(res);
    freeaddrinfo(res);
    return 0;
}

#ifdef TEST_ADDRINFO
int main(int argc, char **argv){

    seeaddrinfo("google.com", "80");
    seeaddrinfo("localhost", "5447");
    return 0;
}
#endif
