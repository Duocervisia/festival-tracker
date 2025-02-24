#ifndef COMPASS_HANDLER_H
#define COMPASS_HANDLER_H

#include <Wire.h>
#include <QMC5883LCompass.h>

class CompassHandler {
public:
    int azimuth;
    bool begin();
    void calibrate();
    int getAzimuth();

private:
    QMC5883LCompass compass;
    float magneticDeclinationDegrees = 0;
};

extern CompassHandler compassHandler;

#endif // COMPASS_HANDLER_H