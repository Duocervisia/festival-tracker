#include "message-handler.h"
#include <Arduino.h>
#include <iostream>

MessageHandler::MessageHandler() : count(0) {
    ownMessage.deviceID = ESP.getEfuseMac() & 0xFFFFFFFF; // Extract lower 32 bits of MAC
    ownMessage.latitude = 0.0;
    ownMessage.longitude = 0.0;
}

void MessageHandler::pushMessage(const struct_message& msg) {
    for (int i = 0; i < count; ++i) {
        if (messages[i].deviceID == msg.deviceID) {
            messages[i] = msg; // Update existing entry
            return;
        }
    }
    if (count < 10) {
        messages[count++] = msg; // Add new entry
    } else {
        std::cerr << "Array is full, cannot add new message." << std::endl;
    }
}

void MessageHandler::printMessages() const {
    for (int i = 0; i < count; ++i) {
        std::cout << "Message " << i << ": "
                  << "Latitude: " << messages[i].latitude
                  << ", Longitude: " << messages[i].longitude
                  << ", DeviceID: " << messages[i].deviceID << std::endl;
    }
}