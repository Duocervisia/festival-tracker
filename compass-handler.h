#ifndef COMPASS_HANDLER_H
#define COMPASS_HANDLER_H

#include <Wire.h>
#include <QMC5883LCompass.h>

class CompassHandler {
public:
    int azimuth;
    bool begin();
    void calibrate();
    void updateAzimuth();

private:
    QMC5883LCompass compass;
    float magneticDeclinationDegrees = 0;
    unsigned long lastAzimuthUpdate = 0;
	unsigned int delayTime = 1000;
    short compassCorrection = 90;
};

extern CompassHandler compassHandler;

#endif // COMPASS_HANDLER_H