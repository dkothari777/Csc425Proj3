# Makefile
#
# Nikolas Gelo & Darshan Kothari
# CSC 425
# Project 3

all: cproxy sproxy

cproxy: cproxy.c debug.h packet.h
	gcc -o cproxy cproxy.c

dcproxy: cproxy.c debug.h packet.h
	gcc -o cproxy cproxy.c -D DEBUG

sproxy: sproxy.c debug.h packet.h
	gcc -o sproxy sproxy.c

dsproxy: cproxy.c debug.h packet.h
	gcc -o sproxy sproxy.c -D DEBUG

clean:
	rm -f *.o core cproxy sproxy

