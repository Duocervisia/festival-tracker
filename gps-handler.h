#include <TinyGPS++.h>

#define GPS_RESET 4

class GpsHandler {
public:
	bool begin();
	void read();
	void reset(unsigned long currentTime);

private:
	const int gpsTxPin = 17;
	const int gpsRxPin = 16;
	TinyGPSPlus gps;
	unsigned long lastPositionUpdate = 0;
	unsigned int delayTime = 1000;
	unsigned int resetTime = 10000;
	unsigned long lastReset = 0;
};