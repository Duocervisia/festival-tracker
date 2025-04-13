#include "communication-handler.h"
#include <ArduinoJson.h>  // JSON serialization for better data handling

// LoRa module setup (adjust the pins accordingly)
// SX1262 lora = new Module(5, 14, 2, 12);

volatile bool CommunicationHandler::operationDone = false; // Define operationDone

CommunicationHandler::CommunicationHandler(Display &disp, MessageHandler &msgHandler) {
    display = &disp;
    messageHandler = &msgHandler;
}

bool CommunicationHandler::begin() {
    Serial.begin(115200);

    int state = lora.begin(434.0, 125.0, 9, 5, 0xAA, 22, 14, 0, false);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRa Initialization Failed, code ");
        Serial.println(state);
        return false;
    }
    pinMode(RECEIVE_LED, OUTPUT);
    lora.setDio1Action(CommunicationHandler::setFlag); // Ensure interrupt is set up
    lora.startReceive(); // Start listening for incoming packets
    Serial.println("LoRa Initializing OK!");

    return true;
}

void CommunicationHandler::sendData() {
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();
    if (!transmitFlag && currentTime - lastSendTime >= delayTime) {
        Serial.println("Sending data...");
        transmitFlag = true;

        // Create a JSON object for structured data transfer
        char buffer[128];
        snprintf(buffer, sizeof(buffer), "%.5f,%.5f,%d,%d", 
                messageHandler->ownMessage.latitude, 
                messageHandler->ownMessage.longitude, 
                messageHandler->ownMessage.deviceID, 
                messageHandler->ownMessage.unixTime);

        // Start transmission (non-blocking)
        transmissionStart = std::chrono::high_resolution_clock::now();

        int transmissionState = lora.startTransmit((uint8_t*)buffer, strlen(buffer) + 1);

        lastSendTime = currentTime;
        delayTime = random(minDelay, maxDelay);
        Serial.println(delayTime);
    }
}

void CommunicationHandler::check(){
    // check if the previous operation finished
    if(operationDone) {
        operationDone = false;
        if(transmitFlag) {
            // the previous operation was transmission, listen for response
            // print the result
            if (transmissionState == RADIOLIB_ERR_NONE) {
                // packet was successfully sent
                auto transmissionEnd = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(transmissionEnd - transmissionStart);
                std::cout << "Transmission took " << duration.count() << " milliseconds." << std::endl;

                // Serial.println(F("transmission finished!"));

            } else {
                Serial.print(F("failed, code "));
                Serial.println(transmissionState);

            }

            // listen for response
            lora.startReceive();
            transmitFlag = false;
        }else{
            checkReceive();
        }
    }
    if(receiveLEDTime != 0 && millis() > receiveLEDTime+receiveLEDDuration){
        digitalWrite(RECEIVE_LED, LOW);
    }
    sendData();
}

void CommunicationHandler::checkReceive() {
    unsigned long currentTime = millis();
    String str;

    int state = lora.readData(str);

    if (state == RADIOLIB_ERR_NONE) {
        lastReceiveTime = currentTime;

        //make gpio2 blink for 500ms
        digitalWrite(RECEIVE_LED, HIGH);
        receiveLEDTime = currentTime + 500;

        Serial.println(F("[SX1262] Received packet!"));

        // Print RSSI and SNR values
        Serial.print(F("[SX1262] RSSI: "));
        Serial.print(lora.getRSSI());
        Serial.println(F(" dBm"));

        Serial.print(F("[SX1262] SNR: "));
        Serial.print(lora.getSNR());
        Serial.println(F(" dB"));

        Serial.print(F("[SX1262] Data: "));
        Serial.println(str);

        // Deserialize JSON for structured data
        // Parse comma-separated string for structured data
        float lat, lon;
        uint32_t id;
        time_t unixTime;

        if (sscanf(str.c_str(), "%f,%f,%u,%ld", &lat, &lon, &id, &unixTime) == 4) {
            struct_message receivedData;
            receivedData.deviceID = id;
            receivedData.latitude = lat;
            receivedData.longitude = lon;
            receivedData.unixTime = unixTime;

            messageHandler->pushMessage(receivedData);

            Serial.printf("Received: ID=%08X, Lat=%.4f, Lon=%.4f, Time=%ld\n", id, lat, lon, unixTime);
        } else {
            Serial.println("Failed to parse data!");
        }

        totalPackets++;
        timeSinceLastPacket = millis();
    } else {
        Serial.println("Failed to receive packet");
    }

    // if (currentTime - lastReceiveTime >= 1000) {
    //     lastReceiveTime = currentTime;
    //     float packetLoss = 100.0 * (10 - (totalPackets - lastTotalPackets)) / 10.0;
    //     lastTotalPackets = totalPackets;
        
    //     char buffer[64];
    //     snprintf(buffer, sizeof(buffer), "ID: %08X\nLoss: %.1f%%\nTotal: %d\nLast: %dms ago", 
    //              deviceID, packetLoss, totalPackets, millis() - timeSinceLastPacket);
    //     display->showText(buffer);
    // }
}

void CommunicationHandler::setFlag(void) {
    operationDone = true;  // Set flag when packet is received
}
