CFLAGS = -O0 -g3 -MMD

addrinfo-t: addrinfo.c
	gcc $(CFLAGS) -DTEST_ADDRINFO $< -o $@

test-addrinfo: addrinfo-t
	./$<

clean:
	rm -f *-t
	rm -f *.o
	rm -f *.d
	rm -f server client
	rm -rf *.dSYM

-include *.d

