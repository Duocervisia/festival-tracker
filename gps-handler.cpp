#include "gps-handler.h"
#include <TinyGPS++.h>

bool GpsHandler::begin() {
    Serial.begin(115200);
    Serial.println("GPS Begin");
    Serial2.begin(9600, SERIAL_8N1, gpsRxPin, gpsTxPin);
    Serial.println("GPS Done!");
    return true;
}

void GpsHandler::read() {
    unsigned long currentTime = millis();
    if (currentTime - lastPositionUpdate >= delayTime) {
        lastPositionUpdate = currentTime;
        Serial.println("Reading GPS");
        Serial.println(Serial2.available());

        while (Serial2.available() > 0) {
            gps.encode(Serial2.read());
        }

        if (gps.location.isUpdated()) {
            Serial.print("Latitude: ");
            Serial.println(gps.location.lat(), 6);
            Serial.print("Longitude: ");
            Serial.println(gps.location.lng(), 6);
            Serial.print("Date: ");
            Serial.print(gps.date.month());
            Serial.print("/");
            Serial.print(gps.date.day());
            Serial.print("/");
            Serial.println(gps.date.year());
            Serial.print("Time: ");
            Serial.print(gps.time.hour());
            Serial.print(":");
            Serial.print(gps.time.minute());
            Serial.print(":");
            Serial.println(gps.time.second());
        }
    }
}
