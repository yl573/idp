
#include "include.h"

using namespace std;

// calibrate sensors
robot::robot() {

	cout << "creating robot" << endl;
	// if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	//   cout << "Cannot initialise link" << endl;
	//   rlink.print_errs("  ");
	// }
}

int getOffset(frontSensorState readings) {
	if(readings == BWW)
		return 1;
	else if(readings == BBW)
		return 2;
	else if(readings == WWB)
		return -1;
	else if(readings == WBB)
		return -2;
	else {
		throw std::invalid_argument( "front sensor readings wrong, check robot.sensors" );
		return 0;
	}
}

void robot::moveForwardUntilJunction() {

	bool approaching = false;
	int lineSpeed;
	int rotationSpeed;
	frontSensorState readings;
	// move while back line following sensor hasn't reached line'
	do {
		sensors.read();
		readings = sensors.getFrontSensorReading();
		// slow down if getting close
		lineSpeed = (readings == WWW) ? 64 : 127; 

		// path correction
		if(readings != BWB) {

			// lost!! disaster recovery
			if(readings == BBB) {
				throw std::runtime_error( "robot got lost!" );	
			}
			// just adjust path
			else {
				int offset = getOffset(readings);
				rotationSpeed = offset * 20;
			}
		} 
		wheels.setStraightRotation(lineSpeed, rotationSpeed);
	} while(!sensors.backSensorOnLine());
	wheels.brake();
}

void robot::turn(int direction) {

	int rotationSpeed;
	frontSensorState slowState1;
	frontSensorState slowState2;

	if(direction == RIGHT) {
		rotationSpeed = 127;
		slowState1 = BBW;
		slowState2 = BWW;
	}
	else if(direction == LEFT) {
		rotationSpeed = -127;
		slowState1 = WBB;
		slowState2 = WWB;
	}
	else
		throw std::invalid_argument( "turning direction can only be left or right" );

	wheels.setStraightRotation(0, rotationSpeed);

	//TODO wait half a second
	// while() {
	// 	sensors.read();
	// }
	frontSensorState currentState;
	int state = 0;
	while(true) {
		sensors.read();
		currentState = sensors.getFrontSensorReading();

		if(currentState == slowState1 && state < 1) {
			rotationSpeed = 30;
			state = 1;
			wheels.setStraightRotation(0, rotationSpeed);
		}
		else if(currentState == slowState2 && state < 2) {
			rotationSpeed = 10;
			state = 2;
			wheels.setStraightRotation(0, rotationSpeed);
		}
		else if(currentState == BWB)
			break;
		else
			throw std::runtime_error( "robot got lost!" );
	}

	wheels.brake();
}

void robot::recovery() {
	cout << "Starting recovery" << endl;
	wheels.brake();
}






	// int rotation;
	// vector<frontSensorState> turningState; // states we are expecting to see
	// if(direction == RIGHT) {
	// 	rotation = 127;
	// 	vector<frontSensorState> temp = {BWB, WWB, WBB, BBB, BBW, BWW, BWB};
	// 	turningState = temp;
	// }
	// else if(direction == LEFT) {
	// 	rotation = -127;
	// 	vector<frontSensorState> temp = {BWB, BWW, BBW, BBB, WBB, WWB, BWB};
	// 	turningState = temp;
	// }
	// else
	// 	throw std::invalid_argument( "turning direction can only be left or right" );

	// int index = 0;
	// do {
	// 	sensors.read();
	// 	frontSensorState currentState = sensors.getFrontSensorReading();

	// 	// state changed
	// 	if(currentState != turningState[index]) {

	// 		// find new state
	// 		int i;
	// 		for(i = index; i < turningState.size(); i++) {
	// 			if(currentState == turningState[i]) {
	// 				break;
	// 			}			 
	// 		}

	// 		// if it is not what we expect, then robot is lost
	// 		if(i == turningState.size())
	// 			throw std::runtime_error( "robot got lost!" );
	// 		else {
	// 			cout << "turningState[" << i << "] reached" << endl;
	// 			index = i;
	// 		}
	// 	}
	// 	wheels.setStraightRotation(0, rotation);
	// } while(index < turningState.size() - 1);




