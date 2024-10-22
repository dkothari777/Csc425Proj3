// cproxy.c
// Nikolas Gelo & Darshan Kothari
// CSC 425
// Project 3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct sockaddr_in LocalTelnetAddress;
struct sockaddr_in SproxyAddress;
struct sockaddr_in ServerTelnetAddress;

int setUpLocalTelnetConnection();
int setUpSproxyConnection(char *ipAddress);
int setUpServerTelnetConnection();
void printUsage(FILE *stream);

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
		
	// Listen for packets from telnet process on this machine. Set the timeout to 100ms.
    // Also, accept the connection request from the local telnet process on this machine.
	listen(localTelnetSocketDescriptor, 100);
    int localTelnetAddressLen = sizeof(LocalTelnetAddress);
	int localTelnetSession = accept(localTelnetSocketDescriptor, (struct sockaddr *) &LocalTelnetAddress, &localTelnetAddressLen);
	if (localTelnetSession < 0) {
		fprintf(stderr, "ERROR on local telnet session accept.\n");
		exit(EXIT_FAILURE);
	}

	// Connect to server telnet daemon.
	if (connect(serverTelnetSocketDescriptor, (struct sockaddr *) &ServerTelnetAddress, sizeof(ServerTelnetAddress)) < 0) {
		fprintf(stderr, "ERROR connecting to server telnet daemon.\n");
		exit(EXIT_FAILURE);
	}

	// Do the initial preparation for the select() implementation.
	//
	// Learned from Beej's Guide to Network Programming.
    fd_set readFileDescriptorSet;
	struct timeval timeout;

	// Continuously check for telnet packets on this machine.
	int localTelnetBytesReceived;
	int serverTelnetBytesReceived;
	char localTelnetBuffer[4096];
	char serverTelnetBuffer[4096];
    int fdsToRead;

	while (1) {
		// Block the thread until either the local telnet connection or the server telnet
		// connection, or both, have sent us data.
        FD_ZERO(&readFileDescriptorSet);
        FD_SET(localTelnetSession, &readFileDescriptorSet);             // Value returned from accept() above in main().
        FD_SET(serverTelnetSocketDescriptor, &readFileDescriptorSet);   // Value returned from socket() in setUpServerTelnetConnection().

        timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int maxFD = (localTelnetSession > serverTelnetSocketDescriptor ? localTelnetSession : serverTelnetSocketDescriptor);
		fdsToRead = select(maxFD+1, &readFileDescriptorSet, NULL, NULL, &timeout);

		// Check for errors in select().
		if (fdsToRead == -1) {
			fprintf(stderr, "ERROR in select().\n");
			exit(EXIT_FAILURE);
		}

		// Check if a timeout occured.
		else if (fdsToRead == 0) {
			printf("Timeout occurred!\n");
			// TODO: Implement heartbeat functionality.
		}

		// Otherwise, there is data to be read from the sockets!
		else {
			// Receive from local telnet session.
			if (FD_ISSET(localTelnetSession, &readFileDescriptorSet)) {
				printf("Will receive from local telnet session.\n");
                memset(localTelnetBuffer, 0, sizeof(localTelnetBuffer));
				localTelnetBytesReceived = recv(localTelnetSession, localTelnetBuffer, sizeof(localTelnetBuffer), 0);
				printf("Did receive from local telnet session: %d\n\n", localTelnetBytesReceived);
				
                // Forward the packet to the server telnet session.
                if (localTelnetBytesReceived > 0) {
                    printf("Will send to server telnet session: %d.\n", localTelnetBytesReceived);
					int sent = send(serverTelnetSocketDescriptor, localTelnetBuffer, localTelnetBytesReceived, 0);
                    localTelnetBytesReceived = 0;
					printf("Did send to server telnet session: %d.\n", sent);
                }
			}

			// Receive from server telnet daemon.
			if (FD_ISSET(serverTelnetSocketDescriptor, &readFileDescriptorSet)) {
				printf("Will receive from server telnet session.\n");
                memset(serverTelnetBuffer, 0, sizeof(serverTelnetBuffer));
				serverTelnetBytesReceived = recv(serverTelnetSocketDescriptor, serverTelnetBuffer, sizeof(serverTelnetBuffer), 0);
				printf("Did receive from server telnet session: %d.\n\n", serverTelnetBytesReceived);
                
				// Forward the packet to the local telnet session.
				if (serverTelnetBytesReceived > 0){
					printf("Will send to local telnet session: %d.\n", serverTelnetBytesReceived);
                    int sent = send(localTelnetSession, serverTelnetBuffer, serverTelnetBytesReceived, 0);
                    serverTelnetBytesReceived = 0;
					printf("Did send to local telnet session: %d.\n", sent);
                }
            }
		}
    }

	return EXIT_SUCCESS;
}

int setUpLocalTelnetConnection()
{
    // Set up the local telnet's address information (here on cproxy).
	memset(&LocalTelnetAddress, 0, sizeof(LocalTelnetAddress)); // 0 out the struct.
    LocalTelnetAddress.sin_family = AF_INET;                    // Domain is the Internet.
    LocalTelnetAddress.sin_addr.s_addr = INADDR_ANY;            // Any IP address will do? Maybe localhost
    LocalTelnetAddress.sin_port = htons(5200);                  // cproxy listens for telnet on port 5200

    // Create the socket.
	int localTelnetSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (localTelnetSocketDescriptor < 0) {
		fprintf(stderr, "ERROR creating local telnet socket.\n");
		exit(EXIT_FAILURE);
	}

	// Bind socket to an address.
	if (bind(localTelnetSocketDescriptor, (struct sockaddr *) &LocalTelnetAddress, sizeof(LocalTelnetAddress)) < 0) {
		fprintf(stderr, "ERROR binding to local telnet socket.\n");
		exit(EXIT_FAILURE);
	}

	return localTelnetSocketDescriptor;
}

int setUpSproxyConnection(char *ipAddress)
{
	// Set up the server's address information (sproxy).
	memset(&SproxyAddress, 0, sizeof(SproxyAddress));           // 0 out the struct.
	SproxyAddress.sin_family = AF_INET;                         // Domain is the Internet.
	SproxyAddress.sin_port = htons(6200);                       // The server listens on port 6200.

	// Set the IP address which was passed in by the user.
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

// Hardcode IP address for telnet address on server & port 23
// for telnet -> cproxy -> daemon debug process.	
int setUpServerTelnetConnection()
{
	// Set up the server's address information (sproxy).
	memset(&ServerTelnetAddress, 0, sizeof(ServerTelnetAddress));   // 0 out the struct
	ServerTelnetAddress.sin_family = AF_INET;                       // Domain is the Internet.
	ServerTelnetAddress.sin_port = htons(23);                       // The server listens on port 6200.

	// Set the IP address (hardcoded at server vm46's IP address).
	if (inet_pton(AF_INET, "192.168.8.2", &ServerTelnetAddress.sin_addr) < 1) {
		fprintf(stderr, "ERROR parsing server telnet IP Address.\n");
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

void printUsage(FILE *stream)
{
	fprintf(stream, "Usage: cproxy <w.x.y.z>\n");
}
