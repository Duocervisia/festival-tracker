#include <TinyGPS++.h>

class GpsHandler {
public:
	bool begin();
	void read();

private:
	const int gpsTxPin = 17;
	const int gpsRxPin = 16;
	TinyGPSPlus gps;
	unsigned long lastPositionUpdate = 0;
	unsigned int delayTime = 1000;
};