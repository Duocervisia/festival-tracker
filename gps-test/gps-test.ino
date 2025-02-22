#include <TinyGPS++.h>

// Define the UART pins for communication with the GPS module
const int gpsTxPin = 17;  // TX pin for GPS
const int gpsRxPin = 16;  // RX pin for GPS

TinyGPSPlus gps;  // Create a TinyGPS++ object

void setup() {
    Serial.begin(115200);  // Initialize the serial monitor for debugging
    Serial.println("GPS Begin");
    Serial2.begin(9600, SERIAL_8N1, gpsRxPin, gpsTxPin);  // Initialize Serial2 with specified RX and TX pins
    Serial.println("GPS Done");
}

void loop() {
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
    delay(1000);
}