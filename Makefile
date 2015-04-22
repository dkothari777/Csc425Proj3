# Makefile
#
# Nikolas Gelo & Darshan Kothari
# CSC 425
# Project 3

all: cproxy sproxy

cproxy: cproxy.c
	gcc -o cproxy cproxy.c

sproxy: sproxy.c
	gcc -o sproxy sproxy.c

clean:
	rm -f *.o core cproxy sproxy

