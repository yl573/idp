
#include "include.h"
#include "robot.h"

using namespace std;

// calibrate sensors
class sensorReader {
public:

	frontSensorState cachedState;

	void read() {
		#ifndef TEST
			reading = rlink.request (READ_PORT_0);
		#else
			reading = 0b01000000;
		#endif

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
		#ifndef TEST
		//rlink.command (RAMP_TIME, 255);
		rlink.command(MOTOR_1_GO, leftDemand);
		rlink.command(MOTOR_2_GO, rightDemand);
		#endif
	}

	void brake() {
		#ifndef TEST
		//rlink.command (RAMP_TIME, 0);
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
}

void robot::moveForwardUntilJunction() {
	cout << "move forward until junction" << endl;
	int lineSpeed;
	int rotationSpeed;
	bool approaching = false;
	frontSensorState readings;
	// move while back line following sensor hasn't reached line'
	do {
		sensors.read();
		readings = sensors.getFrontSensorReading();

		rotationSpeed = 0;

		if(readings == WWW) { // slow down if getting close
			approaching = true;
			rotationSpeed = 0;
		}
<<<<<<< HEAD
		if(readings == BWB) { // path correction
			
=======
		else if(readings == BWB) { // on line
			rotationSpeed = 0;
>>>>>>> 3e5032e7b785ef886508cc2474fde9dbc9d2bc8b
		}
		else if(readings == BBB) { // lost!! disaster recovery		
			//throw runtime_error( "robot got lost!" );
			recovery();
		}		
		else { // just adjust path
			int offset = getOffset(readings);
			rotationSpeed = offset * 20;
		}

		lineSpeed = approaching ? 32 : 64; 
		wheels.setStraightRotation(lineSpeed, rotationSpeed);

	} while(!sensors.backSensorOnLine());
	wheels.brake();
}

void robot::moveBackUntilJunction() {

	int lineSpeed = -32;
	int rotationSpeed;
	frontSensorState readings;
	do {
		sensors.read();
<<<<<<< HEAD
		readings = sensors.getFrontSensorReading(); 

		// path correction
		if(readings != BWB) {

			// lost!! disaster recovery
			if(readings == BBB) {
				//throw runtime_error( "robot got lost!" );
				recovery();	
			}
			// just adjust path
			else {
				int offset = getOffset(readings);
				rotationSpeed = -offset * 20;
			}
		} 
=======
		readings = sensors.getFrontSensorReading();

		if(readings == BWB) { // on line
			rotationSpeed = 0;
		}
		else if(readings == BBB || readings == WWW) { // lost!! disaster recovery		
			cout << "readings: " << readings << endl;
			throw runtime_error( "robot got lost!" );
		}		
		else { // just adjust path
			int offset = getOffset(readings);
			rotationSpeed = offset * -10;
		}

>>>>>>> 3e5032e7b785ef886508cc2474fde9dbc9d2bc8b
		wheels.setStraightRotation(lineSpeed, rotationSpeed);

	} while(!sensors.backSensorOnLine());
	wheels.brake();
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
			if(watch.read() > 100000)
				//throw runtime_error( "robot got lost!" );
				recovery()
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
	if(sensors.cachedState == BWW || sensors.cachedState == BBW) {

	}
	else if (sensors.cachedState == WWB || sensors.cachedState == WBB) {

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




