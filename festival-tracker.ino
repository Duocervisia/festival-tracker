#include <WiFi.h>
#include "display.h"
#include "communication-handler.h"

// Create instances
Display display;
CommunicationHandler commHandler(display);

void setup() {
    Serial.begin(115200);

    if (!display.begin()) {
        Serial.println("Display initialization failed");
        while (true);
    }

    if (!commHandler.begin()) {
        Serial.println("Display initialization failed");
        while (true);
    }
}

void loop() {
    commHandler.sendData();
}
