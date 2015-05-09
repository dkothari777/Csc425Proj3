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
    char payload[1024];
};

struct packet *makeHeartbeatPacket()
{
    struct packet *packet = (struct packet *) malloc(sizeof(struct packet));
    packet->type = PacketTypeHeartbeat;
    packet->payloadLength = 0;
    //memset(packet->payload, 0, sizeof(packet->payload));

    return packet;
}

struct packet *makeApplicationDataPacket(char payload[1024], int payloadLength)
{
    struct packet *packet = (struct packet *) malloc(sizeof(struct packet));
    packet->type = PacketTypeApplicationData;
    packet->payloadLength = payloadLength;
    //packet->payload = payload;
    //packet->payloadLength = payloadLength;
    memset(packet->payload, 0, sizeof(packet->payload));
    memcpy(packet->payload, payload, sizeof(char) * strlen(payload));
    //strcpy(packet->payload, payload);
	//strcat(packet->payload, "\0");
    //packet->payloadLength = strlen(packet->payload);

    return packet;
}

struct packet *packetFromBuffer(char buffer[2048])
{
    struct packet *packet = (struct packet *) malloc(sizeof(struct packet));
    memcpy(packet, buffer, sizeof(struct packet));

    return packet;
}

