
#include "../include/include.h"

using namespace std;

// calibrate sensors
robot::robot() {

	cout << "creating robot" << endl;
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	}
}

int robot::moveUntilJunction(int direction) {

	int speed = 0;
	if(direction == FORWARD) {
		speed = 127;
	}
	else if(direction == BACKWARD) {
		speed = 128 + 127;
	}
	else {
		return ERR_ILLEGAL_VALUE;
	}

	int LFreadings[] = sensors.getLFReading();

	wheels.setSameSpeeds(speed);

	while(!onJunction(LFreadings)) {
		LFreadings = sensors.getLFReading();

		// path correction
		if(!onLine(LFreadings)) {
			
		}
	}

	return SUCCESS;
}

int robot::turn(int direction) {
	wheels.setOppositeSpeeds(127);
	while(true);	
	return 0;
}

bool onLine(int LFreadings[]) {
	return LFreadings[0] == 0 && LFreadings[1] == 1 && LFreadings[2] == 0;
}

bool onJunction(int LFreadings[]) {
	return LFreadings[0] == 1 && LFreadings[1] == 1 && LFreadings[1] == 1;
}

