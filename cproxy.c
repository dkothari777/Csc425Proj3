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

void printUsage(FILE *stream);

int main(int argc, char *argv[])
{
	// TODO: This is where we start the client.

	// Get the IP Address that was passed in.
	if (argc < 2) {
		printUsage(stdout);
		return EXIT_FAILURE;
	}

	// Get information about the server.
	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) < 1) {
		fprintf(stderr, "Error parsing IP Address.\n");
		printUsage(stderr);
		return EXIT_FAILURE;
	}

	// Set the port # to listen to to 5200.
	serverAddress.sin_port = htons(5200);

	// Create the socket.
	int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFileDescriptor < 0) {
		fprintf(stderr, "Error opening socket.\n");
	}

	// Connect to the server.
	if (connect(socketFileDescriptor, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
		fprintf(stderr, "Error connecting to server.\n");
	}

	// TODO: Receive packets

	return EXIT_SUCCESS;
}

void printUsage(FILE *stream)
{
	fprintf(stream, "Usage: cproxy <w.x.y.z>\n");
}

