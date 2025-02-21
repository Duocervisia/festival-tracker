#include <SoftwareSerial.h>

// Define the UART pins for communication with the GPS module
const int gpsTxPin = 17;  // Replace with the actual TX pin number
const int gpsRxPin = 16;  // Replace with the actual RX pin number

SoftwareSerial gpsSerial(gpsRxPin, gpsTxPin);  // Create a SoftwareSerial object

void setup() {
    Serial.begin(115200);  // Initialize the serial monitor for debugging
    Serial.println("GPS Begin");
    gpsSerial.begin(9600);  // Initialize the SoftwareSerial object for GPS communication
    if (!gpsSerial) {
        Serial.println("Failed to initialize GPS Serial");
        while (true);  // Halt the program if initialization fails
    }
    Serial.println("GPS Done");
}

unsigned long previousMillis = 0;  // Stores the last time the loop ran
const long interval = 1000; 

void loop() {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;
        Serial.println("Checking for GPS data...");
        if (gpsSerial.available()) {
            Serial.println("Data available");
            String gpsData = "";
            while (gpsSerial.available()) {
                char c = gpsSerial.read();
                gpsData += c;
                // delay(5);  // Small delay to ensure complete data is read
            }
            Serial.println("Data read");
            // if (gpsData.startsWith("$GNRMC")) {  // Filter for valid NMEA sentences
                Serial.println(gpsData);
            //     parseGNRMC(gpsData);
            // } else {
            //     Serial.println("Invalid data received");
            // }
        } else {
            Serial.println("No data available");
        }
    }
}

void parseGNRMC(String data) {
    Serial.println("Parsing GNRMC data...");
    int commaIndex = data.indexOf(',');
    int nextCommaIndex;
    int fieldIndex = 0;
    String fields[12];

    while (commaIndex != -1 && fieldIndex < 12) {
        nextCommaIndex = data.indexOf(',', commaIndex + 1);
        fields[fieldIndex] = data.substring(commaIndex + 1, nextCommaIndex);
        commaIndex = nextCommaIndex;
        fieldIndex++;
    }

    if (fields[1] == "A") {  // Check if data is valid
        String latitude = fields[2];
        String latHemisphere = fields[3];
        String longitude = fields[4];
        String lonHemisphere = fields[5];

        float latDegrees = latitude.substring(0, 2).toFloat();
        float latMinutes = latitude.substring(2).toFloat();
        float lonDegrees = longitude.substring(0, 3).toFloat();
        float lonMinutes = longitude.substring(3).toFloat();

        float latDecimal = latDegrees + (latMinutes / 60.0);
        float lonDecimal = lonDegrees + (lonMinutes / 60.0);

        if (latHemisphere == "S") latDecimal = -latDecimal;
        if (lonHemisphere == "W") lonDecimal = -lonDecimal;

        Serial.print("Latitude: ");
        Serial.println(latDecimal, 6);
        Serial.print("Longitude: ");
        Serial.println(lonDecimal, 6);
    } else {
        Serial.println("GPS data is invalid (warning).");
    }
}