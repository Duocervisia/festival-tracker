#include "communication-handler.h"

struct_message CommunicationHandler::data;
Display* CommunicationHandler::display = nullptr;
unsigned long CommunicationHandler::lastReceiveTime = 0;
int CommunicationHandler::packetCount = 0;
int CommunicationHandler::totalPackets = 0;

CommunicationHandler::CommunicationHandler(Display &disp) {
    display = &disp;
}

bool CommunicationHandler::begin() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_max_tx_power(82);
    esp_read_mac(ownMac, ESP_MAC_WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed");
        return false;
    }

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
        if (memcmp(peers[i], ownMac, 6) != 0) {
            addPeer(peers[i]);
        }
    }
    return true;
}

void CommunicationHandler::addPeer(uint8_t *peerAddress) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void CommunicationHandler::sendData() {
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();

    if (memcmp(peers[2], ownMac, 6) == 0) {
        delay(1000);
        return;
    }

    if (currentTime - lastSendTime >= 100) {
        lastSendTime = currentTime;

        data.latitude = 52.5200;
        data.longitude = 13.4050;

        for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
            if (memcmp(peers[i], ownMac, 6) == 0) {
                data.device_id = i;
            }
        }

        for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
            if (memcmp(peers[i], ownMac, 6) != 0) {
                esp_now_send(peers[i], (uint8_t *) &data, sizeof(data));
            }
        }
    }
}

void CommunicationHandler::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void CommunicationHandler::OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&data, incomingData, sizeof(data));
    packetCount++;
    totalPackets++;

    unsigned long currentTime = millis();
    if (currentTime - lastReceiveTime >= 1000) {
        float packetLoss = 100.0 * (10 - packetCount) / 10.0; // 10 packets expected per second (100ms interval)

        // Display received data
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "ID: %d\nLoss: %.1f%%\nTotal: %d", data.device_id, packetLoss, totalPackets);
        display->showText(buffer);

        packetCount = 0;
        lastReceiveTime = currentTime;
    }
}