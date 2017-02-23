
#include "include.h"
#include "robot.h"

using namespace std;

// calibrate sensors
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
} sensors;

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
		rlink.command(MOTOR_1_GO, rightDemand);
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
} wheels;

class forkliftDriver {
public:
	int setHeight(int height);
} forklift;



robot::robot() {

	cout << "creating robot" << endl;
	#ifndef TEST
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	}
	#endif
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
				throw runtime_error( "robot got lost!" );	
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

void robot::moveBackUntilJunction() {

	int lineSpeed = -100;
	int rotationSpeed;
	frontSensorState readings;
	do {
		sensors.read();
		readings = sensors.getFrontSensorReading(); 

		// path correction
		if(readings != BWB) {

			// lost!! disaster recovery
			if(readings == BBB) {
				throw runtime_error( "robot got lost!" );	
			}
			// just adjust path
			else {
				int offset = getOffset(readings);
				rotationSpeed = -offset * 20;
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
		throw invalid_argument( "turning direction can only be left or right" );

	wheels.setStraightRotation(0, rotationSpeed);


	#ifndef TEST
	watch.start();
	while(watch.read() < 500) {
		sensors.read();
	}
	watch.stop();
	#endif

	frontSensorState currentState;
	while(true) {
		sensors.read();
		currentState = sensors.getFrontSensorReading();

		if(currentState == slowState1) {
			rotationSpeed = 30;
		}
		else if(currentState == slowState2) {
			rotationSpeed = 10;
		}
		else if(currentState == BWB)
			break;
		else
			throw runtime_error( "robot got lost!" );

		wheels.setStraightRotation(0, rotationSpeed);
	}

	wheels.brake();
}

void robot::recovery() {
	cout << "Starting recovery" << endl;
	wheels.brake();
}

int robot::getOffset(frontSensorState readings) {
	if(readings == BWW)
		return 1;
	else if(readings == BBW)
		return 2;
	else if(readings == WWB)
		return -1;
	else if(readings == WBB)
		return -2;
	else {
		throw invalid_argument( "front sensor readings wrong, check robot.sensors" );
		return 0;
	}
}




