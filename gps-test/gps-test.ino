#include <TinyGPS++.h>

// Define the UART pins for communication with the GPS module
const int gpsTxPin = 17;  // TX pin for GPS
const int gpsRxPin = 16;  // RX pin for GPS

TinyGPSPlus gps;  // Create a TinyGPS++ object

void setup() {
    Serial.begin(115200);  // Debugging baud rate
    Serial.println("GPS Begin");


    Serial2.begin(9600, SERIAL_8N1, gpsRxPin, gpsTxPin); // Start at 9600 baud


    Serial.println("GPS Done");
}

void loop() {
    // Serial2.println("$PMTK225,0*2B");

    Serial.println("Reading GPS");
    Serial.println(Serial2.available());
    while (Serial2.available() > 0) {
        char c = Serial2.read();
        gps.encode(c);
        Serial.print(c);
    }
    Serial.println();

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