// cproxy.c
//
// Nikolas Gelo & Darshan Kothari
// CSC 425
// Project 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void printUsage();

int main(int argc, char *argv[])
{
	// TODO: This is where we start the client.

	// Get the IP Address that was passed in.
	if (argc < 2) {
		printUsage();
		return 1;
	}

	

	return 0;
}

void printUsage()
{
	printf("Usage: cproxy <w.x.y.z>\n");
}

