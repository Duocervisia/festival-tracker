#include <WiFi.h>  // Include this for ESP32

#include "display.h"
#include "message-handler.h"
#include "communication-handler.h"
#include "gps-handler.h"
#include "compass-handler.h"

// Create instances
MessageHandler messageHandler;
Display display;
CommunicationHandler commHandler(display, messageHandler);
GpsHandler gpsHandler;
CompassHandler compassHandler;

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_OFF);
    btStop();
    // adc_power_off();
    // setCpuFrequencyMhz(10);
    uint32_t  Freq = getCpuFrequencyMhz();
    Serial.print("CPU Freq = ");
    Serial.print(Freq);

    // pinMode(DIO1, INPUT_PULLUP);  
    // esp_sleep_enable_ext0_wakeup((gpio_num_t)DIO1, 0); // Wake up on LoRa interrupt

    if (!display.begin()) {
        Serial.println("Display initialization failed");
        while (true);
    }

    if (!commHandler.begin()) {
        Serial.println("Communication handler initialization failed");
        while (true);
    }
    if(!gpsHandler.begin()) {
        Serial.println("GPS handler initialization failed");
        while (true);
    }
    if(!compassHandler.begin()) {
        Serial.println("Compass handler initialization failed");
        while (true);
    }
}

void loop() {
    commHandler.check();
    gpsHandler.read();
    compassHandler.updateAzimuth();

    // Serial.println("Going to light sleep...");
    // esp_light_sleep_start();  // Sleep until LoRa receives something
    // Serial.println("Woke up! Checking LoRa...");
    // delay(1000);
}
