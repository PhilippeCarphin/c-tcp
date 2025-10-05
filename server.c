#include "addrinfo.h"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <arpa/inet.h>

struct args {
    const char *host;
    const char *port;
    int max_conn;
    int argc;
    char **argv;
    FILE *input;
};

void printargv(int argc, char **argv);
int parse_args(int argc, char ** argv, struct args *args);

int tcp_listen(const char *hostname, const char *port, int max_conn)
{
    struct addrinfo *res = addrinfo(hostname, port);
    print_single_addrinfo(res);
    int retval = -1;
    if(res == NULL){
        goto err;
    }
    print_single_addrinfo(res);

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd < 0){
        goto err;
    }

    int err = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if(err){
        perror("bind");
        goto err;
    }

    err = listen(sockfd, max_conn);
    if(err){
        perror("listen");
        goto err;
    }

    retval = sockfd;

err:
    freeaddrinfo(res);
    return retval;
}

int print_peer_name(int conn, int protocol);
int main(int argc, char **argv)
{
    struct args args;
    if(parse_args(argc, argv, &args)){
        return 1;
    }

    int sock_fd = tcp_listen(args.host, args.port, args.max_conn);
    if(sock_fd < 0){
        return 1;
    }
    fprintf(stderr, "Listening on %s args.port %s with %d max connections\n", args.host, args.port, args.max_conn);

    while(1){
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof(their_addr);
        int conn = accept(sock_fd, (struct sockaddr*)&their_addr, &addr_size);
        print_peer_name(conn, their_addr.ss_family);
        while(1){
            // NOTE: nc on raspberry pi requires -N to close the connection
            // on end of input.
            char buf[10];
            size_t nrecv = recv(conn, buf, sizeof(buf),0);
            if(nrecv == 0){
                fprintf(stderr, "Connection closed\n");
                close(conn);
                break;
            }
            fwrite(buf, 1, nrecv, stdout);
        }
    }
    return 0;
}

int print_peer_name(int conn, int protocol){
    struct sockaddr peer_addr;
    socklen_t addr_size = sizeof(peer_addr);
    int err = getpeername(conn, &peer_addr, &addr_size);
    char ipstr[100];
    void *addr;
    char *type;
    switch (protocol){
        case AF_INET:
            addr = &(((struct sockaddr_in*)&peer_addr)->sin_addr);
            type = "IPV4";
            break;
        default:
            addr = &(((struct sockaddr_in6*)&peer_addr)->sin6_addr);
            type = "IPV6";
            break;
    }
    inet_ntop(AF_INET, addr, ipstr, sizeof(ipstr));
    fprintf(stderr, "got connection from %s\n", ipstr);
    return 0;
}

void printargv(int argc, char **argv){
    fprintf(stderr, "%s(): ", __func__);
    for(int i = 1; i<argc; i++){
        fprintf(stderr, "argv[%d]='%s', ", i, argv[i]);
    }
    fprintf(stderr, "\n");
}


static const char *optstring = "io:d::";
static struct option longopts[] = {
    {.name = "host", .has_arg = required_argument, NULL, 'h'},
    {.name = "port", .has_arg = required_argument, NULL, 'p'},
    {.name = "max-conn", .has_arg = required_argument, NULL, 'm'},
    {NULL, 0, NULL, 0},
};

int parse_args(int argc, char ** argv, struct args *args)
{
    args->host =  "localhost";
    args->port = "5447";
    args->max_conn = 10;
    int li;
    char c;
    while((c = getopt_long(argc, argv, optstring, longopts, &li)) != -1){
        struct option opt = longopts[li];
        switch(c){
        case 'h': args->host=optarg ; break;
        case 'p': args->port=optarg ; break;
        case 'm': if(sscanf(optarg, "%d", &args->max_conn) != 1){
                      fprintf(stderr, "Error converting argument of '--%s,-%c': '%s' to integer\n",
                              opt.name, opt.val, optarg);
                      return 1;
                  }
                  if(args->max_conn < 0){
                      fprintf(stderr, "Error argument of '--%s,-%c': %d cannot be negative\n",
                              opt.name, opt.val, args->max_conn);
                      return 1;
                  }
                  break;
        case ':': return -1;
        }
    }
    args->argc = argc - optind;
    args->argv = argv - optind;
    return 0;
}
