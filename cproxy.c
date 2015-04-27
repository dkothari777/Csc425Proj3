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

struct sockaddr_in LocalTelnetAddress;
struct sockaddr_in SproxyAddress;

void printUsage(FILE *stream);
int setUpLocalTelnetConnection();
int setUpSproxyConnection();

int main(int argc, char *argv[])
{
	// Get the IP Address that was passed in.
	if (argc < 2) {
		printUsage(stdout);
		return EXIT_FAILURE;
	}

	// Set up the local telnet & sproxy connections.
	int localTelnetSocketFileDescriptor = setUpLocalTelnetConnection();
	int sproxySocketFileDescriptor = setUpSproxyConnection(sproxyAddress);

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
	memset(&LocalTelnetAddress, 0, sizeof(LocalTelnetAddress)); // 0 out the struct
	LocalTelnetAddress.sin_family = AF_INET; // Domain is the internet.
	LocalTelnetAddress.sin_addr.s_addr = INADDR_ANY; 	//any IP address will do
	LocalTelnetAddress.sin_port = htons(5200); // cproxy listens for telnet on port 5200

	int localTelnetSocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (localTelnetSocketFileDescriptor < 0) {
		fprintf(stderr, "ERROR opening socket.\n");
		exit(EXIT_FAILURE);
	}

	// Bind socket to an address
	if(bind(localTelnetSocketFileDescriptor, (struct sockaddr *) &LocalTelnetAddress, sizeof(localTelnetAddress)) < 0) {
		fprintf(stderr, "ERROR on bind\n");
		exit(EXIT_FAILURE);	
	}

	return localTelnetSocketFileDescriptor;
}

int setUpSproxyConnection()
{
	// Set up the server's address information (sproxy).
	memset(&SproxyAddress, 0, sizeof(SproxyAddress)); // 0 out the struct
	SproxyAddress.sin_family = AF_INET; // Domain is the internet.
	SproxyAddress.sin_port = htons(6200); // The server listens on port 6200.
	if (inet_pton(AF_INET, SproxyAddress, &SproxyAddress.sin_addr) < 1) {
		fprintf(stderr, "Error parsing sproxy IP Address.\n");
		printUsage(stderr);
		exit(EXIT_FAILURE);
	}

	// Create the socket.
	int sproxySocketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (sproxySocketFileDescriptor < 0) {
		fprintf(stderr, "Error opening socket.\n");
		exit(EXIT_FAILURE);
	}

	return sproxySocketFileDescriptor;;
}
