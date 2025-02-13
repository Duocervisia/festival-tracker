#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <U8g2lib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class Display {
public:
    Display();

    bool begin();

    void showText(const char* text);

private:
    U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2;
};

#endif // DISPLAY_H