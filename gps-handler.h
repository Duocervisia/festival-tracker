#ifndef GPS_HANDLER_H
#define GPS_HANDLER_H

#include <TinyGPS++.h>
#include "message-handler.h"

#define GPS_RESET 4
#define EARH_RADIUS 6371

class GpsHandler {
public:
    GpsHandler(MessageHandler &msgHandler);
    bool begin();
    void read();
    void reset(unsigned long currentTime);
    double degreesToRadians(double degrees);
    int calculateDistance(float lat1, float lon1, float lat2, float lon2);
    int calculateAngle(float lat1, float lon1, float lat2, float lon2);

private:
    const int gpsTxPin = 17;
    const int gpsRxPin = 16;
    MessageHandler* messageHandler;
    TinyGPSPlus gps;
    unsigned long lastPositionUpdate = 0;
    unsigned int delayTime = 1000;
    unsigned int resetTime = 30000;
    unsigned long lastReset = 0;
};

#endif // GPS_HANDLER_H