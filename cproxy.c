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
int setUpLocalTelnetConnection();
int setUpSproxyConnection(struct sockaddr_in sproxyAddress);

int main(int argc, char *argv[])
{
	// Get the IP Address that was passed in.
	if (argc < 2) {
		printUsage(stdout);
		return EXIT_FAILURE;
	}

	// Set up the server's address information (sproxy).
	struct sockaddr_in sproxyAddress;
	memset(&sproxyAddress, 0, sizeof(sproxyAddress));
	sproxyAddress.sin_family = AF_INET;
	sproxyAddress.sin_port = htons(6200); // The server listens on port 6200.
	if (inet_pton(AF_INET, sproxyAddress, &sproxyAddress.sin_addr) < 1) {
		fprintf(stderr, "Error parsing sproxy IP Address.\n");
		printUsage(stderr);
		exit(EXIT_FAILURE);
	}

	// Set up the sproxy & local telnet connections.
	int sproxySocketFileDescriptor = setUpSproxyConnection(sproxyAddress);
	int localTelnetSocketFileDescriptor = setUpLocalTelnetConnection();

	// Connect to the server (sproxy).
	if (connect(sproxySocketFileDescriptor, (struct sockaddr *) &sproxyAddress, sizeof(sproxyAddress)) < 0) {
		fprintf(stderr, "Error connecting to server.\n");
	}

	// TODO: Receive packets

	return EXIT_SUCCESS;
}

void printUsage(FILE *stream)
{
	fprintf(stream, "Usage: cproxy <w.x.y.z>\n");
}

int setUpLocalTelnetConnection()
{
	// TODO: Finish the Telnet connection setup.

	return 0;
}

int setUpSproxyConnection(struct sockaddr_in sproxyAddress)
{
	// Create the socket.
	int sproxySocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (sproxySocketFileDescriptor < 0) {
		fprintf(stderr, "Error opening socket.\n");
		exit(EXIT_FAILURE);
	}

	return sproxySocketFileDescriptor;;
}
