
#include "include.h"
#include "robot.h"

using namespace std;

// calibrate sensors
class sensorReader {
public:

	frontSensorState cachedState;

	void read() {
		reading = rlink.request (READ_PORT_0);
		frontSensorState parsedState= getFrontSensorReading();
		if(parsedState != BBB) 
			cachedState = parsedState;
	}

	// get readings from the front sensors
	frontSensorState getFrontSensorReading() {
		bool print = true;
		int frontReadings = reading & 0b00000111;
		switch(frontReadings) {
			case 0b00000010:
				if(print) cout << "BWB" << endl;
				return BWB;
			break;
			case 0b00000110:
				if(print) cout << "WWB" << endl;
				return WWB;
			break;
			case 0b00000100:
				if(print) cout << "WBB" << endl;
				return WBB;
			break;
			case 0b00000011:
				if(print) cout << "BWW" << endl;
				return BWW;
			break;
			case 0b00000001:
				if(print) cout << "BBW" << endl;
				return BBW;
			break;
			case 0b00000000:
				if(print) cout << "BBB" << endl;
				return BBB;
			break;
			case 0b00000111:
				if(print) cout << "WWW" << endl;
				return WWW;
			break;
			default:
				cout << frontReadings << endl;
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
		cout << "straight" << straight << "  rotation" << rotation << endl;
		int leftDemand = clamp(straight + rotation);
		int rightDemand = clamp(-straight + rotation);
		leftDemand = convert(leftDemand);
		rightDemand = convert(rightDemand);
		lastS = straight;
		lastR = rotation;

		cout << "leftDemand" << leftDemand << "  rightDemand" << rightDemand << endl;
		rlink.command(MOTOR_1_GO, leftDemand);
		rlink.command(MOTOR_2_GO, rightDemand);
	}

	void brake() {
		rlink.command(BOTH_MOTORS_GO_SAME, 0);
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
}

void robot::moveForwardUntilJunction() {
	cout << "move forward until junction" << endl;
	int lineSpeed = 127;
	watch.start(); // might continue from last junction, give some time
	do {
		wheels.setStraightRotation(lineSpeed, getRotationDemand());
	} while(watch.read() < 1000 || !sensors.backSensorOnLine());

	wheels.brake();
}

void robot::moveBackUntilJunction() {
	cout << "move back until junction" << endl;
	int lineSpeed = -127;
	do {
		wheels.setStraightRotation(lineSpeed, -getRotationDemand());
	} while(!sensors.backSensorOnLine());
	wheels.brake();
}

void robot::moveBackUntilFrontOnLine() {
	cout << "move back until front on line" << endl;
	int lineSpeed = -127;

	do {
		wheels.setStraightRotation(lineSpeed, -getRotationDemand());
	} while(!sensors.getFrontSensorReading() != WWW);

	wheels.brake();
}

int robot::getRotationDemand() { 
	sensors.read();
	frontSensorState readings = sensors.getFrontSensorReading(); 
	if(readings == BWB) 
		return 0;
	else if(readings == BBB) {
		// lost!! disaster recovery
		recovery();
		return 0;
	}
	else {
		// just adjust path
		int offset = getOffset(readings);
		return offset * 20;
	}
}


void robot::turn(int direction) {

	int rotationSpeed = 127;
	frontSensorState slowState1;
	frontSensorState slowState2;

	if(direction == RIGHT) {
		slowState1 = BBW;
		slowState2 = BWW;
	}
	else if(direction == LEFT) {
		slowState1 = WBB;
		slowState2 = WWB;
	}
	else
		throw invalid_argument( "turning direction can only be left or right" );

	wheels.setStraightRotation(0, rotationSpeed);

	frontSensorState currentState;
	watch.start();
	while(true) {
		rotationSpeed = 127;
		sensors.read();
		currentState = sensors.getFrontSensorReading();

		if(currentState == slowState1) {
			rotationSpeed = 64;
		}
		else if(currentState == slowState2) {
			rotationSpeed = 32;
		}
		else if(currentState == BWB) {
			cout << "turn complete" << endl;
			break;
		}
		else {
			if(watch.read() > 10000)
				//throw runtime_error( "robot got lost!" );
				recovery();
		}

		if(direction == LEFT) 
			rotationSpeed *= -1;
		wheels.setStraightRotation(0, rotationSpeed);
	}

	wheels.brake();
}

void robot::recovery() {
	cout << "Starting recovery" << endl;
	wheels.brake();
	int rotationSpeed = 64; // if cachedState is not reliable, take a leap of faith
	//int lineSpeed = 64;
	if(sensors.cachedState == BWW || sensors.cachedState == BBW) {
		rotationSpeed = 64;
	}
	else if (sensors.cachedState == WWB || sensors.cachedState == WBB) {
		rotationSpeed = -64;
	}
	//frontSensorState reading;

	// S-shaped search
	// An error is throw when a line is found
	try {
		while(true) {
			// timeouts should make it turn 90 degrees
			wheels.setStraightRotation(0, rotationSpeed);
			waitTimeoutOrReachedLine(1000);
			wheels.setStraightRotation(0, -rotationSpeed);
			waitTimeoutOrReachedLine(2000);
		}
	} catch(runtime_error& error) {
		wheels.brake();
		cout << error.what() << endl;
	}

	// after it finds a line
	moveForwardUntilJunction();
}

// throws exception when a line is found
void robot::waitTimeoutOrReachedLine(int timeout) {
	watch.start();
	while(watch.read() < timeout) { 
		sensors.read();
		frontSensorState reading = sensors.getFrontSensorReading();
		if(reading != BBB)
			throw runtime_error( "line is found!" );
	}
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
		cout << readings << endl;
		throw invalid_argument( "front sensor readings wrong, check robot.sensors" );
		return 0;
	}
}




