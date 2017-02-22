
#include "include.h"
using namespace std;

//Deals with calibration

class wheelsDriver {
public:
	// +ve forward and right, -ve back and left
	void setStraightRotation(int straight, int rotation) {
		int leftDemand = clamp(straight + rotation);
		int rightDemand = clamp(straight - rotation);
		
	}

	void brake() {

	}

private:
	int clamp(int num) {
		if(num > 127) {
			return 127;
		}
		else if(num < -127) {
			return -127;
		}
		return num;
	}
};
