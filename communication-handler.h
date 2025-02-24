#ifndef COMMUNICATION_HANDLER_H
#define COMMUNICATION_HANDLER_H

#include <RadioLib.h>
#include "display.h"
#include "message-handler.h"
#include <queue>
#include <chrono>

#define NSS 5
#define RESET 13
#define DIO1 14
#define BUSY 27

class CommunicationHandler {
public:
    CommunicationHandler(Display &disp, MessageHandler &msgHandler);
    bool begin();
    void sendData();
    void checkReceive();
    void check();
    void sendPacket();
    static void setFlag(void);
private:
    Display* display = nullptr;
    MessageHandler* messageHandler = nullptr;
    unsigned long lastReceiveTime = 0;
    unsigned int lastTotalPackets = 0;
    unsigned int totalPackets = 0;
    unsigned int timeSinceLastPacket = 0;
    unsigned short delayTime = 2000;
    unsigned short minDelay = 1000;
    unsigned short maxDelay = 3000;
    std::chrono::time_point<std::chrono::high_resolution_clock> transmissionStart;
    static volatile bool operationDone;
    bool transmitFlag = false;
    bool scanFlag = false;
    int transmissionState = RADIOLIB_ERR_NONE; // Ensure transmissionState is declared
    SX1262 lora = new Module(NSS, RESET, DIO1, BUSY);
};

#endif // COMMUNICATION_HANDLER_H