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
int setUpSproxyConnection(char *sproxyIPAddress);

int main(int argc, char *argv[])
{
	// Get the IP Address that was passed in.
	if (argc < 2) {
		printUsage(stdout);
	}

	// Set up the sproxy & local telnet connections.
	int sproxySocketFileDescriptor = setUpSproxyConnection(argv[1]);
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
	exit(EXIT_FAILURE);
}

int setUpLocalTelnetConnection()
{
	// TODO: Finish the Telnet connection setup.

	return 0;
}

int setUpSproxyConnection(char *sproxyIPAddress)
{
	// Get information about the sproxy server.
	struct sockaddr_in sproxyAddress;
	memset(&sproxyAddress, 0, sizeof(sproxyAddress));
	sproxyAddress.sin_family = AF_INET;
	if (inet_pton(AF_INET, sproxyIPAddress, &sproxyAddress.sin_addr) < 1) {
		fprintf(stderr, "Error parsing sproxy IP Address.\n");
		printUsage(stderr);
	}

	// The server listens on port 6200.
	sproxyAddress.sin_port = htons(6200);

	// Create the socket.
	int sproxySocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (sproxyFileDescriptor < 0) {
		fprintf(stderr, "Error opening socket.\n");
		exit(EXIT_FAILURE);
	}

	return sproxySocketFileDescriptor;;
}
