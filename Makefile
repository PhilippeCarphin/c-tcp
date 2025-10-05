CFLAGS = -O0 -g2 -MMD
LDFLAGS= -L.
CC = gcc # gcc-14
AR = gcc-ar-14

libobj = addrinfo.o

all: test-server

libnet.a: $(libobj)
	$(AR) -crs $@ $^

server: server.o libnet.a
	$(CC) $(LDFLAGS) -o $@ $< -lnet

addrinfo-t: addrinfo.c
	$(CC) -O0 -g3 -DTEST_ADDRINFO $< -o $@

test-addrinfo: addrinfo-t
	./$<

test-server: server
	./$< $(ARGS)

clean:
	rm -f *-t
	rm -f *.o
	rm -f *.d
	rm -f server client
	rm -rf *.dSYM
	rm -f *.a

-include *.d

