// packet.h
//
// Nikolas Gelo & Darshan Kothari
// CSC 425
// Project 3

#define PacketTypeHeartbeat 0
#define PacketTypeNewConnection 1
#define PacketTypeApplicationData 2

struct packet {
    int type;
    int payloadLength;
    char payload[4096];
};

struct packet *makeHeartbeatPacket()
{
    struct packet *packet = malloc(sizeof(packet));
    packet->type = PacketTypeHeartbeat;
    packet->payloadLength = 0;
    memset(packet->payload, 0, sizeof(packet->payload));

    return packet;
}

struct packet *makeApplicationDataPacket(char *payload, int payloadLength)
{
    struct packet *packet = malloc(sizeof(packet));
    packet->type = PacketTypeApplicationData;
    packet->payloadLength = payloadLength;
    memset(packet->payload, 0, sizeof(packet->payload));
    memcpy(packet->payload, payload, payloadLength);

    return packet;
}

