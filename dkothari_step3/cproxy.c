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
#include "debug.h"
#include "packet.h"

struct sockaddr_in LocalTelnetAddress;
struct sockaddr_in SproxyAddress;

int setUpLocalTelnetConnection();
int setUpSproxyConnection(char *ipAddress);
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
	int sproxySocketDescriptor = setUpSproxyConnection(argv[1]);
		
	// Listen for packets from telnet process on this machine. Set the timeout to 100ms.
    // Also, accept the connection request from the local telnet process on this machine.
	listen(localTelnetSocketDescriptor, 100);
    int localTelnetAddressLen = sizeof(LocalTelnetAddress);
	int localTelnetSession = accept(localTelnetSocketDescriptor, (struct sockaddr *) &LocalTelnetAddress, &localTelnetAddressLen);
	if (localTelnetSession < 0) {
		fprintf(stderr, "ERROR on local telnet session accept.\n");
		exit(EXIT_FAILURE);
	}

	// Connect to sproxy.
	if (connect(sproxySocketDescriptor, (struct sockaddr *) &SproxyAddress, sizeof(SproxyAddress)) < 0) {
		fprintf(stderr, "ERROR connecting to sproxy.\n");
		exit(EXIT_FAILURE);
	}

	// Do the initial preparation for the select() implementation.
	//
	// Learned from Beej's Guide to Network Programming.
    fd_set readFileDescriptorSet;
	struct timeval timeout;

	// Continuously check for telnet packets on this machine.
	int localTelnetBytesReceived;
	int sproxyBytesReceived;
	char localTelnetBuffer[2048];
	char sproxyBuffer[2048];
    char sendBuffer[2048];
    int fdsToRead;
    
    struct packet * rbuffer = (struct packet *) malloc(sizeof(struct packet));
    struct packet * sbuffer = (struct packet *) malloc(sizeof(struct packet));

	while (1) {
		// Block the thread until either the local telnet connection or the server telnet
		// connection, or both, have sent us data.
        FD_ZERO(&readFileDescriptorSet);
        FD_SET(localTelnetSession, &readFileDescriptorSet);             // Value returned from accept() above in main().
        FD_SET(sproxySocketDescriptor, &readFileDescriptorSet);   // Value returned from socket() in setUpServerTelnetConnection().

        timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		int maxFD = (localTelnetSession > sproxySocketDescriptor ? localTelnetSession : sproxySocketDescriptor);
		fdsToRead = select(maxFD+1, &readFileDescriptorSet, NULL, NULL, &timeout);

		// Check for errors in select().
		if (fdsToRead == -1) {
			fprintf(stderr, "ERROR in select().\n");
			exit(EXIT_FAILURE);
		}

		// Check if a timeout occured.
		else if (fdsToRead == 0) {
			DLog("Timeout occurred!\n");
			// TODO: Implement heartbeat functionality.
            exit(EXIT_SUCCESS);
		}

		// Otherwise, there is data to be read from the sockets!
		else {
			// Receive from local telnet.
			if (FD_ISSET(localTelnetSession, &readFileDescriptorSet)) {
                memset(localTelnetBuffer, 0, sizeof(localTelnetBuffer));
				localTelnetBytesReceived = recv(localTelnetSession, localTelnetBuffer, sizeof(localTelnetBuffer), 0);
				DLog("Did receive from local telnet: %d\n", localTelnetBytesReceived);
				
                //Convert buffer to packet
                memset(sbuffer->payload, 0, sizeof(sbuffer->payload));
                memcpy(sbuffer->payload, localTelnetBuffer, localTelnetBytesReceived);
                sbuffer->pLength = localTelnetBytesReceived;

                //conver packet to SendBuffer
                memset(sendBuffer, 0, sizeof(sendBuffer));
                memcpy(sendBuffer, sbuffer, sizeof(struct packet));
                
                // Forward the packet to sproxy.
                if (localTelnetBytesReceived > 0) {
					int sent = send(sproxySocketDescriptor, sendBuffer, sizeof(struct packet), 0);
                    localTelnetBytesReceived = 0;
					DLog("Did send to sproxy session: %d.", sent);
                }
			}

			// Receive from sproxy.
			if (FD_ISSET(sproxySocketDescriptor, &readFileDescriptorSet)) {
                memset(sproxyBuffer, 0, sizeof(sproxyBuffer));
				sproxyBytesReceived = recv(sproxySocketDescriptor, sproxyBuffer, sizeof(sproxyBuffer), 0);
				DLog("Did receive from server telnet: %d.\n", sproxyBytesReceived);
                

                //Convert sproxyBuffer to packet
                memcpy(rbuffer, sproxyBuffer, sizeof(struct packet));

				// Forward the packet to the local telnet.
				if (sproxyBytesReceived > 0){
                    int sent = send(localTelnetSession, rbuffer->payload, rbuffer->pLength, 0);
                    sproxyBytesReceived = 0;
					DLog("Did send to local telnet: %d.\n", sent);
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

void printUsage(FILE *stream)
{
	fprintf(stream, "Usage: cproxy <w.x.y.z>\n");
}
