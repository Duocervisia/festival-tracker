#include "communication-handler.h"

struct_message CommunicationHandler::data;
Display* CommunicationHandler::display = nullptr;
unsigned long CommunicationHandler::lastReceiveTime = 0;
int CommunicationHandler::lastTotalPackets = 0;
int CommunicationHandler::totalPackets = 0;
int CommunicationHandler::timeSinceLastPacket = 0;

CommunicationHandler::CommunicationHandler(Display &disp) {
    display = &disp;
}

bool CommunicationHandler::begin() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_protocol((wifi_interface_t)ESP_IF_WIFI_STA, WIFI_PROTOCOL_LR);
    esp_read_mac(ownMac, ESP_MAC_WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Initialization Failed");
        return false;
    }

    // esp_now_register_send_cb(OnDataSent);
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
        return;
    }

    if (currentTime - lastSendTime >= 100) {
        Serial.println("Sending data");

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
        lastSendTime = currentTime;

    }
}

// void CommunicationHandler::OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//     Serial.print("Last Packet Send Status: ");
//     Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

void CommunicationHandler::checkReceive() {
    if (memcmp(peers[2], ownMac, 6) != 0) {
        return;
    }
    unsigned long currentTime = millis();
    if (currentTime - lastReceiveTime >= 1000) {
        lastReceiveTime = currentTime;
        float packetLoss = 100.0 * (10 - (totalPackets - lastTotalPackets)) / 10.0;
        lastTotalPackets = totalPackets;
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "ID: %d\nLoss: %.1f%%\nTotal: %d\nLast: %dms ago", data.device_id, packetLoss, totalPackets, millis() - timeSinceLastPacket);
        display->showText(buffer);
    }
}

void CommunicationHandler::OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&data, incomingData, sizeof(data));
    totalPackets++;
    timeSinceLastPacket = millis();
}