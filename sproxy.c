// sproxy.c
//
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

struct sockaddr_in CproxyAddress;
struct sockaddr_in TelnetDaemonAddress;

int setUpCproxyConnection();
int setUpTelnetDaemonConnection();

int main(int argc, char *argv[])
{
    // Set up the cproxy & telnet daemon connections.
    int cproxySocketDescriptor = setUpCproxyConnection();
    int telnetDaemonSocketDescriptor = setUpTelnetDaemonConnection();

	return EXIT_SUCCESS;
}

int setUpCproxyConnection()
{
    // Set up the cproxy's address information (on cproxy).
    memset(&CproxyAddress, 0, sizeof(CproxyAddress);                // 0 out the struct.
    CproxyAddress.sin_family = AF_INET;                             // Domain is the Internet.
    CproxyAddress.sin_addr.s_addr = INADDR_ANY;                     // Any incoming IP address will do.
    CproxyAddress.sin_port = htons(6200);                           // sproxy listens for cproxy on port 6200.

    // Create the socket.
    int cproxySocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (cproxySocketDescriptor < 0) {
        fprintf(stderr, "ERROR creating cproxy socket.\n");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to an address.
    if (bind(cproxySocketDescriptor, (struct sockaddr *) &CproxyAddress, sizeof(CproxyAddress)) < 0) {
        fprintf(stderr, "ERROR binding to cproxy socket.\n");
        exit(EXIT_FAILURE);
    }

    return cproxySocketDescriptor;
}

int setUpTelnetDaemonConnection()
{
    // Set up the telnet daemon's address information (here on sproxy).
    memset(&TelnetDaemonAddress, 0, sizeof(TelnetDaemonAddress));   // 0 out the struct.
    TelnetDaemonAddress.sin_family = AF_INET;                       // Domain is the Internet.
    TelnetDaemonAddress.sin_port = htons(23);                       // The telnet daemon listens on port 23.

    // Set the IP address.
    if (inet_pton(AF_INET, "127.0.0.1", &TelnetDaemonAddress.sin_addr) < 1) {
        fprintf(stderr, "ERROR parsing telnet daemon address.\n");
        exit(EXIT_FAILURE);
    }

    // Create the socket.
    int telnetDaemonSocketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (telnetDaemonSocketDescriptor < 0) {
        fprintf(stderr, "ERROR creating telnet daemon socket.\n");
        exit(EXIT_FAILURE);
    }

    return telnetDaemonSocketDescriptor;
}

