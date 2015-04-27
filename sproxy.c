// sproxy.c
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

void setupTelnetConnection();
int setupSproxyServer();


int main(int argc, char *argv[])
{
	// TODO: This is where we start the server.

	return 0;
}

void setupTelenetConnection(){
	//TODO: NEED to talk to teacher about this
	return;
}

int setupSproxyServer(){

	struct sockaddr_in sproxyAddr;	
	// create a stream socket(TCP)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd <0)
	{
		fprintf(stderr, "ERROR opening socket");
		exit(EXIT_FAILURE);
	}	
	// 0 out the struct
	memset(&sproxyAddr, 0, sizeof(sproxyAddr));

	//domain is the internet
	sproxyAddr.sin_family = AF_INET;

	//any IP address will do
	sproxyAddr.sin_addr.s_addr = INADDR_ANY;

	//set port number 
	sproxyAddr.sin_port = htons(6200);

	//bind socket to an address
	if(bind(sockfd, (struct sockaddr *), &sproxyAddr, sizeof(sproxyAddr)) < 0)
	{
		fprintf(stderr, "ERROR on bind");
		exit(EXIT_FAILURE);	
	}
	return sockfd;
}

