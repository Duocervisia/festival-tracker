#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "display.h" // Include the Display class

// Geben Sie die MAC-Adressen der anderen drei Geräte hier ein
uint8_t peers[][6] = {
    {0xCC, 0xDB, 0xA7, 0x2F, 0xEF, 0xB4},
    {0xE4, 0x65, 0xB8, 0x77, 0x9B, 0xF4},
    {0xD4, 0x8A, 0xFC, 0x96, 0x28, 0x68}, // Firebeetle 2
};

uint8_t own_mac[6];

// Struktur für das Senden von Daten
typedef struct struct_message {
    float latitude;
    float longitude;
    int device_id;
} struct_message;

struct_message myData;

// Create an instance of the Display class
Display display;

// Rückruffunktion, wenn Daten gesendet werden
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Rückruf für Empfang
unsigned long lastReceiveTime = 0;
int packetCount = 0;
int totalPackets = 0;

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    packetCount++;
    totalPackets++;

    unsigned long currentTime = millis();
    if (currentTime - lastReceiveTime >= 1000) {
        float packetLoss = 100.0 * (10 - packetCount) / 10.0; // 10 packets expected per second (100ms interval)
        
        // Serial.print("Packets received in the last second: ");
        // Serial.println(packetCount);
        // Serial.print("Packet loss (%): ");
        // Serial.println(packetLoss);

        // Display received data
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "ID: %d\nLoss: %.1f%%\nTotal: %d", myData.device_id, packetLoss, totalPackets);
        display.showText(buffer);

        packetCount = 0;
        lastReceiveTime = currentTime;
    }

    // Serial.print("Bytes received: ");
    // Serial.println(len);
    // Serial.print("Latitude: ");
    // Serial.println(myData.latitude);
    // Serial.print("Longitude: ");
    // Serial.println(myData.longitude);
    // Serial.print("Device ID: ");
    // Serial.println(myData.device_id);
}

void setup() {
    Serial.begin(115200);

    // Initialize display
    if (!display.begin()) {
        Serial.println("Display initialization failed");
        while (true); // Stop execution if display initialization fails
    }

    // WiFi im station mode
    WiFi.mode(WIFI_STA);

    // Set WiFi to long-range mode
    esp_wifi_set_max_tx_power(82); // 82 corresponds to maximum power (20.5 dBm)

    // ESP-NOW initialisieren
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_read_mac(own_mac, ESP_MAC_WIFI_STA);
    
    // Rückrufe registrieren
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // Peers hinzufügen
    for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
        if(memcmp(peers[i], own_mac, 6) != 0){
            addPeer(peers[i]);
        }
    }
}

void addPeer(uint8_t *peerAddress) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void loop() {
    static unsigned long lastSendTime = 0;
    unsigned long currentTime = millis();

    if (memcmp(peers[2], own_mac, 6) == 0) {
        sleep(1000);
    }

    if (currentTime - lastSendTime >= 100) {
        lastSendTime = currentTime;

        // Dummy GPS-Koordinaten und Geräte-ID senden
        myData.latitude = 52.5200; // Beispielkoordinate für Berlin
        myData.longitude = 13.4050; // Beispielkoordinate für Berlin

        // Set device id based on own position of mac address in peers array
        for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
            if (memcmp(peers[i], own_mac, 6) == 0) {
                myData.device_id = i;
            }
        }

        // Serial.print("Device ID: ");
        // Serial.println(myData.device_id);

        // Senden Sie die Nachricht an alle Peers
        for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
            if (memcmp(peers[i], own_mac, 6) != 0) {
                esp_err_t result = esp_now_send(peers[i], (uint8_t *) &myData, sizeof(myData));
                // Serial.print("Send Status: ");
                // Serial.println(result == ESP_OK ? "Success" : "Error");
            }
        }
    }
}