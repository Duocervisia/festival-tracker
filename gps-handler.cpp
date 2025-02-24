#include "gps-handler.h"
#include <TinyGPS++.h>

HardwareSerial neogps(1);

bool GpsHandler::begin() {
    Serial.begin(115200);
    Serial.println("GPS Begin");
    neogps.begin(9600, SERIAL_8N1, gpsRxPin, gpsTxPin);
    delay(1000);
    neogps.println("$PMTK314,-1*04");
    neogps.println("$PMTK220,1000*1F");

    Serial.println("GPS Done!");
    return true;
}

void GpsHandler::read() {
    unsigned long currentTime = millis();
    if (currentTime - lastPositionUpdate >= delayTime) {
        lastPositionUpdate = currentTime;
        Serial.println("Reading GPS");
        Serial.println(neogps.available());
        boolean newData = false;

        while (neogps.available()){
            char c = neogps.read();
            Serial.print(c);
            if (gps.encode(c))
            {
                newData = true;
            }
        }
        Serial.println();

        //If newData is true
        if(newData == true)
        {
            newData = false;
            Serial.println(gps.satellites.value());
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
