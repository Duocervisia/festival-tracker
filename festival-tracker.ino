#include "display.h"
#include "communication-handler.h"
#include "gps-handler.h"

// Create instances
Display display;
CommunicationHandler commHandler(display);
GpsHandler gpsHandler;

void setup() {
    Serial.begin(115200);

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
}

void loop() {
    commHandler.check();
    gpsHandler.read();
}
