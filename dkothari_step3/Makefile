# Makefile
#
# Nikolas Gelo & Darshan Kothari
# CSC 425
# Project 3

all: cproxy sproxy

cproxy: cproxy.c debug.h packet.h
	gcc -Wall -g -std=c99 -o cproxy cproxy.c -D DEBUG

sproxy: cproxy.c debug.h packet.h
	gcc -Wall -g -std=c99 -o sproxy sproxy.c -D DEBUG

clean:
	rm -f *.o core cproxy sproxy

