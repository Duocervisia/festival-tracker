#include <esp_now.h>
#include <WiFi.h>


// Geben Sie die MAC-Adressen der anderen drei Geräte hier ein
uint8_t peers[][6] = {
    {0xCC, 0xDB, 0xA7, 0x2F, 0xEF, 0xB4},
    {0xE4, 0x65, 0xB8, 0x77, 0x9B, 0xF4},
};

// Struktur für das Senden von Daten
typedef struct struct_message {
    float latitude;
    float longitude;
    int device_id;
} struct_message;

struct_message myData;

// Rückruffunktion, wenn Daten gesendet werden
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Last Packet Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Rückruf für Empfang
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Latitude: ");
    Serial.println(myData.latitude);
    Serial.print("Longitude: ");
    Serial.println(myData.longitude);
    Serial.print("Device ID: ");
    Serial.println(myData.device_id);
}

void setup() {
    Serial.begin(115200);

    // WiFi im station mode
    WiFi.mode(WIFI_STA);

    // Set WiFi to long-range mode
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

    // ESP-NOW initialisieren
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Rückrufe registrieren
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);

    // Peers hinzufügen
    for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
        addPeer(peers[i]);
    }
}

void addPeer(uint8_t *peerAddress) {
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
    }
}

void loop() {
    // Dummy GPS-Koordinaten und Geräte-ID senden
    myData.latitude = 52.5200; // Beispielkoordinate für Berlin
    myData.longitude = 13.4050; // Beispielkoordinate für Berlin

    //set device id based on own position of mac adress in peers array
    uint8_t own_mac[6];
    esp_read_mac(own_mac, ESP_MAC_WIFI_STA);
    for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
        if (memcmp(peers[i], own_mac, 6) == 0) {
            myData.device_id = i;
        }
    }

    Serial.print("Device ID: ");
    Serial.println(myData.device_id);

    // Senden Sie die Nachricht an alle Peers
    for (int i = 0; i < sizeof(peers) / sizeof(peers[0]); i++) {
        if (memcmp(peers[i], own_mac, 6) != 0) {
            esp_err_t result = esp_now_send(peers[i], (uint8_t *) &myData, sizeof(myData));
            Serial.print("Send Status: ");
            Serial.println(result == ESP_OK ? "Success" : "Error");
        }
    }

    delay(2000);
}