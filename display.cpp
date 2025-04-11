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
void updateDisplay() {
    
}