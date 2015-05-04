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
struct sockaddr_in ServerTelnetAddress;

void printUsage(FILE *stream);
int setUpLocalTelnetConnection();
int setUpSproxyConnection(char *ipAddress);
int setUpServerTelnetConnection();

int main(int argc, char *argv[])
{
	// Get the IP Address that was passed in.
	if (argc < 2) {
		printUsage(stdout);
		exit(EXIT_FAILURE);
	}

	// Set up the local telnet & sproxy connections.
	int localTelnetSocketDescriptor = setUpLocalTelnetConnection();
	// int sproxySocketDescriptor = setUpSproxyConnection(argv[1]);
	int serverTelnetSocketDescriptor = setUpServerTelnetConnection();
	
	// Connect to server telnet daemon.
	if (connect(serverTelnetSocketDescriptor, (struct sockaddr *) &ServerTelnetAddress, sizeof(ServerTelnetAddress)) < 0) {
		fprintf(stderr, "ERROR connecting to server telnet daemon.\n");
		exit(EXIT_FAILURE);
	}
	
	// Listen for packets from telnet process on this machine. Set the timeout to 100ms.
	listen(localTelnetSocketDescriptor, 100);
	int localTelnetAddressLen = sizeof(LocalTelnetAddress);
	int localTelnetSession = accept(localTelnetSocketDescriptor, (struct sockaddr *) &LocalTelnetAddress, &localTelnetAddressLen);
	if (localTelnetSession < 0) {
		fprintf(stderr, "ERROR on local telnet session accept.\n");
		exit(EXIT_FAILURE);
	}
	
	// Continuously check for telnet packets on this machine.
	int bytesReceived;
	uint32_t *localTelnetBuffer = malloc(sizeof(uint32_t));
	uint32_t *serverTelnetBuffer = malloc(sizeof(uint32_t));
	while (1) {
		// TODO: Implement select() functionality.

		// Receive from local telnet session.
		printf("Will receive from local telnet session.\n");
		bytesReceived = recv(localTelnetSession, localTelnetBuffer, sizeof(uint32_t), 0);
		printf("Did receive from local telnet session.\n\n");
		if (bytesReceived < 0) {
			fprintf(stderr, "ERROR on reading from local telnet session.\n");
			exit(EXIT_FAILURE);
		} else {
			// Forward buffer to the server telnet daemon.
			printf("Will send to server telnet session.\n");
			send(serverTelnetSocketDescriptor, localTelnetBuffer, sizeof(uint32_t), 0);
			printf("Did send to server telnet session.\n\n");
		}

		// Receive from server telnet daemon.
		printf("Will receive from server telnet session.\n");
		bytesReceived = recv(serverTelnetSocketDescriptor, serverTelnetBuffer, sizeof(uint32_t), 0);
		printf("Did receive from server telnet session.\n\n");
		if (bytesReceived < 0) {
			fprintf(stderr, "ERROR on reading from server telnet session.\n");
			exit(EXIT_FAILURE);
		} else {
			// Forward buffer to the local telnet daemon.
			printf("Will send to local telnet session.\n");
			send(localTelnetSession, serverTelnetBuffer, sizeof(uint32_t), 0);
			printf("Did send to local telnet session.\n\n");
		}
	}

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
	LocalTelnetAddress.sin_addr.s_addr = INADDR_ANY; // Any IP address will do? Maybe localhost
	LocalTelnetAddress.sin_port = htons(5200); // cproxy listens for telnet on port 5200

	int localTelnetSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (localTelnetSocketDescriptor < 0) {
		fprintf(stderr, "ERROR creating local telnet socket.\n");
		exit(EXIT_FAILURE);
	}

	// Bind socket to an address
	if (bind(localTelnetSocketDescriptor, (struct sockaddr *) &LocalTelnetAddress, sizeof(LocalTelnetAddress)) < 0) {
		fprintf(stderr, "ERROR binding to local telnet socket.\n");
		exit(EXIT_FAILURE);
	}

	return localTelnetSocketDescriptor;
}

int setUpSproxyConnection(char *ipAddress)
{
	// Set up the server's address information (sproxy).
	memset(&SproxyAddress, 0, sizeof(SproxyAddress)); // 0 out the struct
	SproxyAddress.sin_family = AF_INET; // Domain is the internet.
	SproxyAddress.sin_port = htons(6200); // The server listens on port 6200.

	// Set the IP address
	if (inet_pton(AF_INET, ipAddress, &SproxyAddress.sin_addr) < 1) {
		fprintf(stderr, "ERROR parsing sproxy IP Address.\n");
		printUsage(stderr);
		exit(EXIT_FAILURE);
	}

	// Create the socket.
	int sproxySocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (sproxySocketDescriptor < 0) {
		fprintf(stderr, "ERROR creating sproxy socket.\n");
		exit(EXIT_FAILURE);
	}

	return sproxySocketDescriptor;
}

int setUpServerTelnetConnection()
{
	// Hardcode IP address for telnet address on server & port 23
	// for telnet -> cproxy -> daemon debug process.
	
	// Set up the server's address information (sproxy).
	memset(&ServerTelnetAddress, 0, sizeof(ServerTelnetAddress)); // 0 out the struct
	ServerTelnetAddress.sin_family = AF_INET; // Domain is the internet.
	ServerTelnetAddress.sin_port = htons(23); // The server listens on port 6200.

	// Set the IP address
	if (inet_pton(AF_INET, "192.168.8.2", &ServerTelnetAddress.sin_addr) < 1) {
		fprintf(stderr, "ERROR parsing sproxy IP Address.\n");
		printUsage(stderr);
		exit(EXIT_FAILURE);
	}

	// Create the socket.
	int serverTelnetSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (serverTelnetSocketDescriptor < 0) {
		fprintf(stderr, "ERROR creating server telnet socket.\n");
		exit(EXIT_FAILURE);
	}

	return serverTelnetSocketDescriptor;
}
