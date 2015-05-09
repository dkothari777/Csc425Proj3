// packet.h
//
// Nikolas Gelo & Darshan Kothari
// CSC 425
// Project 3
#include <arpa/inet.h>
#include "debug.h"

#define PacketTypeHeartbeat 1
#define PacketTypeNewConnection 2
#define PacketTypeApplicationData 3

struct packet {
    int type;
    int pLength;
    char payload[512];
};
