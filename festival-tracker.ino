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
    // gpsHandler.read();
    // compassHandler.getAzimuth();

    // delay(1000);
}
