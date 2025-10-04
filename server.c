#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> // Struct addrinfo

#include <arpa/inet.h>
#include <string.h> // memset
#include <errno.h>
#include <stdio.h>
#define print_field(pfx, a, t, field) do { fprintf(stderr, #pfx ":" #a "->" #field "=%" #t "\n", a->field); } while(0)

int print_sockaddr(struct sockaddr *s)
{
    fprintf(stderr, "%s", s->sa_data);
    fprintf(stderr, "%d", s->sa_len);
    return 0;
}
int print_addrinfo(struct addrinfo *a){
    // a->ai_addr;
    // a->ai_protocol;
    // a->ai_canonname;
    print_field(addrinfo, a, d, ai_family);
    print_field(addrinfo, a, d, ai_protocol);
    print_field(addrinfo, a, s, ai_canonname);
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
    fprintf(stderr, "%s: %s\n", type, ipstr);
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
    for(struct addrinfo *cur = res; cur->ai_next != NULL; cur = cur->ai_next){
        print_addrinfo(cur);
    }
    return res;
}

#ifdef TEST_ADDRINFO
int main(int argc, char **argv){

    struct addrinfo *res = addrinfo("google.com", "80");
    if(addrinfo("localhost", "5447")){
        return 1;
    }
    freeaddrinfo(res);

    return 0;
}
#endif
