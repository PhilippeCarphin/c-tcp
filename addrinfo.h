#ifndef ADDRINFO_H
#define ADDRINFO_H
int print_single_addrinfo(struct addrinfo *a);
int print_addrinfo(struct addrinfo *head);
struct addrinfo *addrinfo(const char *hostname, const char *port);
int seeaddrinfo(const char *hostname, const char *port);
#endif
