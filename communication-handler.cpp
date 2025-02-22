#include "communication-handler.h"

CommunicationHandler::CommunicationHandler(Display &disp) {
    display = &disp;
    deviceID = ESP.getEfuseMac() & 0xFFFFFFFF; // Extract lower 32 bits of MAC
}

bool CommunicationHandler::begin() {
    Serial.begin(115200);
    
    int state = lora.begin(434.0, 125.0, 9, 7, 0x12, 10, 8, 0, false);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRa Initialization Failed, code ");
        Serial.println(state);
        return false;
    }
    
    lora.setSyncWord(0xAA); // Set sync word for network separation
    Serial.println("LoRa Initializing OK!");
    return true;
}

void CommunicationHandler::sendData() {
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastSendTime >= delayTime) {
        Serial.println("Sending data");

        data.latitude = 52.5200;
        data.longitude = 13.4050;
        data.deviceID = deviceID; // Store unique device ID in struct

        int state =  lora.transmit((uint8_t*)&data, sizeof(data));
        // Check for transmission errors
        if (state == RADIOLIB_ERR_NONE) {
            Serial.println("Transmission successful!");
        } else {
            Serial.print("Transmission failed, code ");
            Serial.println(state);
        }

        lastSendTime = currentTime;
    }
}

void CommunicationHandler::checkReceive() {
    unsigned long currentTime = millis();

    while (int packetSize = lora.receive((uint8_t*)&data, sizeof(data))) {
        if (packetSize == sizeof(data)) {
            OnDataRecv(packetSize);
        } else {
            Serial.println("Received packet of unexpected size");
        }
    }
    
    if (currentTime - lastReceiveTime >= 1000) {
        lastReceiveTime = currentTime;
        float packetLoss = 100.0 * (10 - (totalPackets - lastTotalPackets)) / 10.0;
        lastTotalPackets = totalPackets;
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "ID: %08X\nLoss: %.1f%%\nTotal: %d\nLast: %dms ago", 
                 data.deviceID, packetLoss, totalPackets, millis() - timeSinceLastPacket);
        display->showText(buffer);
    }
}

void CommunicationHandler::OnDataRecv(int packetSize) {
    if (packetSize == sizeof(data)) {
        totalPackets++;
        timeSinceLastPacket = millis();
    } else {
        Serial.println("Received packet of unexpected size");
    }
}
