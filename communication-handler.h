#ifndef COMMUNICATION_HANDLER_H
#define COMMUNICATION_HANDLER_H

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "display.h"

struct struct_message {
    float latitude;
    float longitude;
    int device_id;
};

class CommunicationHandler {
public:
    CommunicationHandler(Display &disp);
    bool begin();
    void sendData();
    void checkReceive();

private:
    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);
    void addPeer(uint8_t *peerAddress);
    
    static struct_message data;
    static Display* display;

    static unsigned long lastReceiveTime;
    static int packetCount;
    static int totalPackets;

    uint8_t ownMac[6];
    uint8_t peers[3][6] = {
        {0xCC, 0xDB, 0xA7, 0x2F, 0xEF, 0xB4},
        {0xE4, 0x65, 0xB8, 0x77, 0x9B, 0xF4},
        {0xD4, 0x8A, 0xFC, 0x96, 0x28, 0x68},
    };
};

#endif
