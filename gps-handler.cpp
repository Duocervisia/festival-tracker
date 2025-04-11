#include "gps-handler.h"
#include <TinyGPS++.h>
#include <ctime>

HardwareSerial neogps(1);

GpsHandler::GpsHandler(MessageHandler &msgHandler){
    messageHandler = &msgHandler;
}

bool GpsHandler::begin() {
    Serial.begin(115200);
    Serial.println("GPS Begin");
    neogps.begin(9600, SERIAL_8N1, gpsRxPin, gpsTxPin);
    delay(1000);
    neogps.println("$PMTK314,-1*04");
    neogps.println("$PMTK220,1000*1F");
    pinMode(GPS_RESET, OUTPUT);
    digitalWrite(GPS_RESET, HIGH);


    Serial.println("GPS Done!");
    return true;
}

void GpsHandler::reset(unsigned long currentTime) {
    if (currentTime - lastReset >= resetTime) {
        lastReset = currentTime;
        Serial.println("Resetting GPS");
        digitalWrite(GPS_RESET, LOW);
        delay(100);
        digitalWrite(GPS_RESET, HIGH);
    }
}

double GpsHandler::degreesToRadians(double degrees) { 
    return degrees * M_PI / 180.0; 
}

int GpsHandler::calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    lat1 = degreesToRadians(lat1);
    lon1 = degreesToRadians(lon1);
    lat2 = degreesToRadians(lat2);
    lon2 = degreesToRadians(lon2);

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
                std::cos(lat1) * std::cos(lat2) * std::sin(dlon / 2.0) *
                    std::sin(dlon / 2.0);
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

    // Distance in kilometers
    double distance = EARH_RADIUS * c * 1000.0;  // Convert to meters

    return distance;
}

int GpsHandler::calculateAngle(float lat1, float lon1, float lat2, float lon2) {
    lat1 = degreesToRadians(lat1);
    lon1 = degreesToRadians(lon1);
    lat2 = degreesToRadians(lat2);
    lon2 = degreesToRadians(lon2);

    // Calculate the differences
    double dLon = lon2 - lon1;

    // Calculate the initial bearing using the Haversine formula
    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - (sin(lat1) * cos(lat2) * cos(dLon));
    double initialBearing = atan2(y, x);

    // Convert the initial bearing from radians to degrees
    initialBearing = fmod((initialBearing + 2.0 * M_PI), (2.0 * M_PI));
    initialBearing = initialBearing * (180.0 / M_PI);
    // Adjust the angle to be relative to the north (clockwise from positive
    // X-axis)
    initialBearing = 90.0 - initialBearing;

    // Ensure the result is in the range [0, 360)
    if (initialBearing < 0.0) {
        initialBearing += 360.0;
    }

    return initialBearing;
}

void GpsHandler::read() {
    unsigned long currentTime = millis();
    if (currentTime - lastPositionUpdate >= delayTime) {
        lastPositionUpdate = currentTime;
        Serial.println("Reading GPS");
        Serial.println(neogps.available());
        boolean newData = false;

        if(neogps.available() == 0){
            reset(currentTime);
        }

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

            int year = gps.date.year();
            int month = gps.date.month();
            int day = gps.date.day();
            int hour = gps.time.hour();
            int minute = gps.time.minute();
            int second = gps.time.second();

            // Create a tm structure
            struct tm t;
            t.tm_year = year - 1900; // tm_year is years since 1900
            t.tm_mon = month - 1;    // tm_mon is 0-based
            t.tm_mday = day;
            t.tm_hour = hour;
            t.tm_min = minute;
            t.tm_sec = second;
            t.tm_isdst = -1;         // Not considering daylight saving time

            // Convert to time_t (Unix time)
            time_t unixTime = mktime(&t);
            Serial.print("Unix Time: ");
            Serial.println(unixTime);

            messageHandler->updateOwnMessage(gps.location.lat(), gps.location.lng(), unixTime);
        }
    }
}
