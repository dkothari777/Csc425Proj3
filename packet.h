// packet.h
//
// Nikolas Gelo & Darshan Kothari
// CSC 425
// Project 3

#include "debug.h"

#define PacketTypeHeartbeat 1
#define PacketTypeNewConnection 2
#define PacketTypeApplicationData 3

struct packet {
    int type;
    int payloadLength;
    char payload[4096];
};

struct packet *makeHeartbeatPacket()
{
    struct packet *packet = malloc(sizeof(struct packet));
    packet->type = PacketTypeHeartbeat;
    packet->payloadLength = 0;
    //memset(packet->payload, 0, sizeof(packet->payload));

    return packet;
}

struct packet *makeApplicationDataPacket(char payload[4096], int payloadLength)
{
    struct packet *packet = malloc(sizeof(struct packet));
    packet->type = PacketTypeApplicationData;
    packet->payloadLength = payloadLength;
    //memset(packet->payload, 0, sizeof(packet->payload));
    //memcpy(packet->payload, payload, payloadLength);
    strncpy(packet->payload, payload, strlen(payload)+1);

    return packet;
}

