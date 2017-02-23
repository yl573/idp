
#include "include.h"
using namespace std;

//Deals with calibration

class wheelsDriver {
public:
	int lastS, lastR;


	// +ve forward and right, -ve back and left
	void setStraightRotation(int straight, int rotation) {
		if(lastS == straight && lastR == rotation)
			return;
		int leftDemand = clamp(straight + rotation);
		int rightDemand = clamp(straight - rotation);
		leftDemand = convert(leftDemand);
		rightDemand = convert(rightDemand);
		lastS = straight;
		lastR = rotation;


		#ifndef TEST
		rlink.command (RAMP_TIME, 255);
		rlink.command(MOTOR_1_GO, leftDemand);
		rlink.command(MOTOR_2_GO, rightDemand);
		#endif
	}

	void brake() {
		#ifndef TEST
		rlink.command (RAMP_TIME, 0);
		rlink.command(BOTH_MOTORS_GO_SAME, 0);
		#endif
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

	int convert(int num) {
		if(num < 0) {
			num = 127 - num;
		}
		return num;
	}
};
