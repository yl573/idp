
#include "include.h"
using namespace std;

enum frontSensorState {BWB, WWB, WBB, BWW, BBW, BBB, WWW};

class sensorReader {
public:

	void read() {
		#ifndef TEST
			reading = rlink.request (READ_PORT_0);
		#endif
	}

	// get readings from the front sensors
	frontSensorState getFrontSensorReading() {
		int frontReadings = reading & 0b00000111;
		switch(frontReadings) {
			case 0b00000010:
				return BWB;
			break;
			case 0b00000110:
				return WWB;
			break;
			case 0b00000100:
				return WBB;
			break;
			case 0b00000011:
				return BWW;
			break;
			case 0b00000001:
				return BBW;
			break;
			case 0b00000000:
				return BBB;
			break;
			case 0b00000111:
				return WWW;
			break;
			default:
				throw std::invalid_argument( "invalid sensor reading" );
		}
	}

	// get reading from back sensor
	bool backSensorOnLine() {
		return reading & 00001000;
	}

	int getWeightReading(int &weight) {
		return 0;
	}

	int getColorReading(int &color) {
		return 0;
	}

private:
	int reading;
};
