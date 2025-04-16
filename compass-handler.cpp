#include "compass-handler.h"

bool CompassHandler::begin() {
    //values from https://www.magnetic-declination.com/#
    int degrees = 4;
    int minutes = 58;
    magneticDeclinationDegrees = degrees + minutes / 60;
    compass.init();
    compass.setCalibrationOffsets(-254.00, 788.00, -321.00);
    compass.setCalibrationScales(0.89, 1.10, 1.03);
    Serial.println("Compass initialized");
    return true;
}

void CompassHandler::calibrate() {
    compass.init();

    Serial.println(
        "This will provide calibration settings for your QMC5883L chip. When "
        "prompted, move the magnetometer in all directions until the calibration "
        "is complete.");
    Serial.println("Calibration will begin in 5 seconds.");
    delay(5000);

    Serial.println("CALIBRATING. Keep moving your sensor. It will take 10s...");
    compass.calibrate();

    Serial.println(
        "DONE. Copy the lines below and paste it into your projects sketch.);");
    Serial.println();
    Serial.print("compass.setCalibrationOffsets(");
    Serial.print(compass.getCalibrationOffset(0));
    Serial.print(", ");
    Serial.print(compass.getCalibrationOffset(1));
    Serial.print(", ");
    Serial.print(compass.getCalibrationOffset(2));
    Serial.println(");");
    Serial.print("compass.setCalibrationScales(");
    Serial.print(compass.getCalibrationScale(0));
    Serial.print(", ");
    Serial.print(compass.getCalibrationScale(1));
    Serial.print(", ");
    Serial.print(compass.getCalibrationScale(2));
    Serial.println(");");
}

void CompassHandler::updateAzimuth() {
    unsigned long currentTime = millis();
    if(currentTime - lastAzimuthUpdate < delayTime){
        return;
    }
    lastAzimuthUpdate = currentTime;

    compass.read();
    int x = compass.getX();
    int y = compass.getY();

    float heading = atan2(x, y) * 180.0 / M_PI;
    heading += magneticDeclinationDegrees;

    // heading += 360;
    // heading += compassCorrection;
    int iheading = (int)heading % 360;

    if (iheading < 0) {
        iheading = 360 + iheading;
    }
    iheading += 70;
    azimuth = iheading % 360;
    Serial.println("Azimuth: " + String(azimuth));

}

