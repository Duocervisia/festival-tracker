#ifndef COMMUNICATION_HANDLER_H
#define COMMUNICATION_HANDLER_H

#include <RadioLib.h>
#include "display.h"
#include <queue>

#define NSS 5
#define RESET 14
#define DIO1 2
#define BUSY 12

struct struct_message {
    float latitude;
    float longitude;
    uint32_t deviceID; // Unique ID based on ESP efuse MAC
};

class CommunicationHandler {
public:
    CommunicationHandler(Display &disp);
    bool begin();
    void sendData();
    void checkReceive();
    void OnDataRecv(int packetSize);

private:
    struct_message data;
    struct_message receivedData[10];
    Display* display = nullptr;
    unsigned long lastReceiveTime = 0;
    unsigned int lastTotalPackets = 0;
    unsigned int totalPackets = 0;
    unsigned int timeSinceLastPacket = 0;
    unsigned int delayTime = 1000;
    uint32_t deviceID; // Store unique device ID
    SX1262 lora = new Module(NSS, RESET, DIO1, BUSY);
    std::queue<struct_message> packetQueue; // Buffer for received packets
};

#endif // COMMUNICATION_HANDLER_H