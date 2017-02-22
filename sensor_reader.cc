#pragma once
#include "include.h"
using namespace std;

class sensorReader {
public:
	int getLFReading() {
		int readings[4] = {0,1,0,0};
		return readings;
	}

	int getWeightReading(int &weight) {
		return 0;
	}

	int getColorReading(int &color) {
		return 0;
	}
};