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
#include "debug.h"
#include "packet.h"

struct sockaddr_in CproxyAddress;
struct sockaddr_in TelnetDaemonAddress;

int setUpCproxyConnection();
int setUpTelnetDaemonConnection();

int main(int argc, char *argv[])
{
    // Set up the cproxy & telnet daemon connections.
    int cproxySocketDescriptor = setUpCproxyConnection();
    int telnetDaemonSocketDescriptor = setUpTelnetDaemonConnection();

    // Connect to the telnet daemon on this server.
    if (connect(telnetDaemonSocketDescriptor, (struct sockaddr *) &TelnetDaemonAddress, sizeof(TelnetDaemonAddress)) < 0) {
        fprintf(stderr, "ERROR on connecting to telnet daemon.\n");
        exit(EXIT_FAILURE);
    }

    // Listen for packets from cproxy on the client server. Set the timeout to 10s.
    // Also, accept the connection request from cproxy.
    listen(cproxySocketDescriptor, 10000);
    int cproxyAddressLen = sizeof(CproxyAddress);
    int cproxySession = accept(cproxySocketDescriptor, (struct sockaddr *) &CproxyAddress, &cproxyAddressLen);
    if (cproxySession < 0) {
        fprintf(stderr, "ERROR on cproxy session accept.\n");
        exit(EXIT_FAILURE);
    }

    // Do the initial preparation for select().
    fd_set readFileDescriptorSet;
    struct timeval timeout;

    // Continuously check for telnet packets on this machine.
    int cproxyBytesReceived;
    int telnetDaemonBytesReceived;
    char cproxyBuffer[2048];
    char telnetDaemonBuffer[2048];
    char sendBuffer[2048];
    int fdsToRead;

    struct packet * rbuffer = (struct packet*) malloc(sizeof(struct packet)); 
    struct packet * sbuffer = (struct packet*) malloc(sizeof(struct packet));
    while (1) {
        // Block the thread until either the cproxy connection or the telnet daemon
        // connection, or both, have sent us data.
        FD_ZERO(&readFileDescriptorSet);
        FD_SET(cproxySession, &readFileDescriptorSet);                  // Value returned from accept() above in main().
        FD_SET(telnetDaemonSocketDescriptor, &readFileDescriptorSet);   // Value returned from socket() in setUpTelnetDaemonConnection().

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        int maxFD = (cproxySession > telnetDaemonSocketDescriptor ? cproxySession : telnetDaemonSocketDescriptor);
        fdsToRead = select(maxFD+1, &readFileDescriptorSet, NULL, NULL, &timeout);

        // Check for errors in select().
        if (fdsToRead == -1) {                                                                                                              
            fprintf(stderr, "ERROR in select().\n");                                                                                        
            exit(EXIT_FAILURE);                                                                                                             
        }                                                                                                                                   
                                                                                                                                            
        // Check if a timeout occured.                                                                                                      
        else if (fdsToRead == 0) {                                                                                                          
            DLog("Timeout occurred!");
            // TODO: Implement heartbeat functionality.                                                                                     
        }

        // Otherwise, there is data to be read from the sockets!
        else {
            // Receive from cproxy.
            if (FD_ISSET(cproxySession, &readFileDescriptorSet)) {
                memset(cproxyBuffer, 0, sizeof(cproxyBuffer));
                cproxyBytesReceived = recv(cproxySession, cproxyBuffer, sizeof(cproxyBuffer), 0);
                DLog("Did receive from cproxy: %d\n", cproxyBytesReceived);

                //convert buffer to packet and send the packet
                //memset(rbuffer, 0, sizeof(struct packet)); //this may bring errors!!!!!
                memcpy(rbuffer, cproxyBuffer, sizeof(struct packet));
                
                // Forward the packet to the telnet daemon.
                if (cproxyBytesReceived > 0) {
                    int sent = send(telnetDaemonSocketDescriptor, rbuffer->payload, rbuffer->pLength, 0);
                    cproxyBytesReceived = 0;
                    DLog("Did send to telnet daemon: %d.\n", sent);
                }
            }

            // Receive from telnet daemon.
            if (FD_ISSET(telnetDaemonSocketDescriptor, &readFileDescriptorSet)) {
                memset(telnetDaemonBuffer, 0, sizeof(telnetDaemonBuffer));
                telnetDaemonBytesReceived = recv(telnetDaemonSocketDescriptor, telnetDaemonBuffer, sizeof(telnetDaemonBuffer), 0);
                DLog("Did receive from telnet daemon: %d.\n", telnetDaemonBytesReceived);

                //telnetDaemonBuffer Convert to packet 
                //memset(sbuffer, 0, sizeof(struct packet)); //this may bring errors!!!!!!
                memset(sbuffer->payload, 0, sizeof(sbuffer->payload));
                memcpy(sbuffer->payload, telnetDaemonBuffer, telnetDaemonBytesReceived);
                sbuffer->pLength = telnetDaemonBytesReceived;

                //packet memcopy to send buffer
                memset(sendBuffer, 0, sizeof(sendBuffer));
                memcpy(sendBuffer, sbuffer, sizeof(struct packet));
                
                // Forward the packet to cproxy.
                if (telnetDaemonBytesReceived > 0){
                    int sent = send(cproxySession, sendBuffer, sizeof(struct packet), 0);
                    telnetDaemonBytesReceived = 0;
                    DLog("Did send to local telnet session: %d.\n", sent);
                }
            }   
        }
    }

    return EXIT_SUCCESS;
}

int setUpCproxyConnection()
{
    // Set up the cproxy's address information (on cproxy).
    memset(&CproxyAddress, 0, sizeof(CproxyAddress));               // 0 out the struct.
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

