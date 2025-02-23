#include "communication-handler.h"
#include <ArduinoJson.h>  // JSON serialization for better data handling

// LoRa module setup (adjust the pins accordingly)
// SX1262 lora = new Module(5, 14, 2, 12);

volatile bool CommunicationHandler::operationDone = false; // Define operationDone

CommunicationHandler::CommunicationHandler(Display &disp) {
    display = &disp;
    deviceID = ESP.getEfuseMac() & 0xFFFFFFFF; // Extract lower 32 bits of MAC
}

bool CommunicationHandler::begin() {
    Serial.begin(115200);

    // pinMode(RESET, OUTPUT);
    // digitalWrite(RESET, LOW);
    // delay(10);
    // digitalWrite(RESET, HIGH);
    // delay(10);
   
    int state = lora.begin(434.0, 125.0, 9, 7, 0xAA, 10, 8, 0, false);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRa Initialization Failed, code ");
        Serial.println(state);
        return false;
    }
    
    // lora.setSyncWord(0xAA); // Set sync word for network separation
    // lora.setDio1Action(CommunicationHandler::setFlag); // Ensure interrupt is set up
    lora.startReceive(); // Start listening for incoming packets
    Serial.println("LoRa Initializing OK!");
    operationDone = true;

    return true;
}

void CommunicationHandler::sendData() {
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastSendTime >= delayTime) {
        Serial.println("Sending data...");

        // Create a JSON object for structured data transfer
        StaticJsonDocument<128> json;
        json["latitude"] = 52.5200;
        json["longitude"] = 13.4050;
        json["deviceID"] = deviceID;

        //print json
        Serial.print("Sending: ");
        serializeJson(json, Serial);
        Serial.println();

        char buffer[128];
        serializeJson(json, buffer);

        // Start transmission (non-blocking)
        int transmissionState = lora.transmit((uint8_t*)buffer, strlen(buffer) + 1);
        if (transmissionState == RADIOLIB_ERR_NONE) {
            Serial.println(F("transmission finished!"));

        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);

        }
        lora.startReceive(); // Start listening for incoming packets

        lastSendTime = currentTime;
    }
}

void CommunicationHandler::check(){
    // check if the previous operation finished

    checkReceive();  
    sendData();
    
}

void CommunicationHandler::checkReceive() {
    unsigned long currentTime = millis();
    String str;
    int state = lora.readData(str);

    if (state == RADIOLIB_ERR_NONE) {
        // Serial.println(F("[SX1262] Received packet!"));

        if (lora.getRSSI() < -120 || lora.getSNR() < -20) {
            // Serial.println("Packet ignored due to low signal quality.");
            return;
        }

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
        StaticJsonDocument<128> json;
        DeserializationError error = deserializeJson(json, str);
        if (!error) {
            float lat = json["latitude"];
            float lon = json["longitude"];
            uint32_t id = json["deviceID"];

            Serial.printf("Received: ID=%08X, Lat=%.4f, Lon=%.4f\n", id, lat, lon);
        } else {
            Serial.println("Failed to parse JSON!");
        }

        totalPackets++;
        timeSinceLastPacket = millis();
    } else {
        Serial.println("Failed to receive packet");
    }

    if (currentTime - lastReceiveTime >= 1000) {
        lastReceiveTime = currentTime;
        float packetLoss = 100.0 * (10 - (totalPackets - lastTotalPackets)) / 10.0;
        lastTotalPackets = totalPackets;
        
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "ID: %08X\nLoss: %.1f%%\nTotal: %d\nLast: %dms ago", 
                 deviceID, packetLoss, totalPackets, millis() - timeSinceLastPacket);
        display->showText(buffer);
    }
}

void CommunicationHandler::setFlag(void) {
    operationDone = true;  // Set flag when packet is received
}
