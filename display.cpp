#include <Wire.h>
#include <U8g2lib.h>
#include "display.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Display::Display(GpsHandler &gpsHdlr, MessageHandler &msgHdlr) : u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE) {
    gpsHandler = &gpsHdlr;
    messageHandler = &msgHdlr;
}

bool Display::begin() {
    u8g2.begin();
    u8g2.clearBuffer(); // Clear the buffer
    return true;
}

void Display::showText(const char* text) {
    u8g2.clearBuffer(); // Clear the buffer
    u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font

    // Split the text into lines and draw each line separately
    int y = 12; // Starting y position
    const char* line = text;
    while (*line) {
        const char* nextLine = strchr(line, '\n');
        if (nextLine) {
            u8g2.drawStr(0, y, line);
            line = nextLine + 1;
        } else {
            u8g2.drawStr(0, y, line);
            break;
        }
        y += 12; // Move to the next line (adjust as needed for your font size)
    }

    u8g2.sendBuffer(); // Show the display buffer on the screen
}

void Display::updateDirection(int azimuth) {
    double ownLatitude = messageHandler->ownMessage.latitude;
    double ownLongitude = messageHandler->ownMessage.longitude;

    // Serial.print("Count: ");
    // Serial.println(messageHandler->count);

    for (int i = 0; i < messageHandler->count; i++) {
        double targetLatitude = messageHandler->messages[i].latitude;
        double targetLongitude = messageHandler->messages[i].longitude;

        // Calculate the direction to the target
        double deltaLongitude = targetLongitude - ownLongitude;
        double x = cos(targetLatitude) * sin(deltaLongitude);
        double y = cos(ownLatitude) * sin(targetLatitude) - sin(ownLatitude) * cos(targetLatitude) * cos(deltaLongitude);
        double direction = atan2(x, y) * 180.0 / M_PI;

        // Adjust the direction based on the azimuth
        direction -= azimuth; // Subtract azimuth to align with the target
        if (direction < 0) {
            direction += 360; // Normalize to 0-360 degrees
        } else if (direction >= 360) {
            direction -= 360; // Normalize to 0-360 degrees
        }

        // Print the direction
        // Serial.print("Direction: ");
        // Serial.println(direction);
    }
}