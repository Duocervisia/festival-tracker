#include <Wire.h>
#include <U8g2lib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSH1106 display connected to I2C (SDA, SCL pins)
U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE);


void setup() {
    // initialize with the I2C addr 0x3C (for the 128x64)
    u8g2.begin();
    u8g2.clearBuffer(); // Clear the buffer

    // Set font
    u8g2.setFont(u8g2_font_ncenB08_tr); // Choose a suitable font

    // Draw a string at position (0, 24)
    u8g2.drawStr(0, 24, "Hello, World!");

    // Show the display buffer on the screen
    u8g2.sendBuffer();
}

void loop() {
    // put your main code here, to run repeatedly:
}