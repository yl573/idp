
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
	map<int,string> portMap{ {0,”READ_PORT_0”}, {1,”READ_PORT_1”} , {2,”READ_PORT_2”} , {3,”READ_PORT_3”}, {4,”READ_PORT_4”}, {5,”READ_PORT_5”}}; //can I use string?
	//enum PortRead{READ_PORT_0, READ_PORT_1, READ_PORT_2, READ_PORT_3, READ_PORT_4, READ_PORT_5}
	int lastHeight;//reference (shouldn't it be global?)
	int currentHeight; //could read all the sensors to know what's the current height but it's wasteful
	bool aligned = 0;

	forkliftDriver(int h){ lastHeight = h;}

	bool read(int n) { //copied from sensor reader, assume sensor outputs bool
		#ifndef TEST
		string port = portMap.at(n)
		reading = rlink.request (port); //get the port numbers
		return reading;
		#endif
	}
	//set the new height as last height inside setHight(int) or outide?    MOTOR_3 for forklift
	int setHeight(int height){
		if (height == lastHeight){
			if (read(height) != true){
				while (true){
					rlink.command(MOTOR_3_GO, 127);
					if(read(0) == 1){lastHeight = 0; break;}
					if(read(1) == 1){lastHeight = 1; break;}
					if(read(2) == 1){lastHeight = 2; break;}
					if(read(3) == 1){lastHeight = 3; break;}
					if(read(4) == 1){lastHeight = 4; break;}
					if(read(5) == 1){lastHeight = 5; break;}
			}
		}
		if (height > lastHeight){
			#ifndef TEST
			while (aligned == false){
				rlink.command(MOTOR_3_GO, 127);
				aligned = read(height);
			}
			#endif
		}
		if (height < lastHeight){
			#ifndef TEST
			while (aligned == false){
				rlink.command(MOTOR_3_GO, 255);
				aligned = read(height);
				#endif
			}
		}
		#ifndef TEST
		rlink.command (RAMP_TIME, 0);//quick stopping, have to stop while the sensor still reads 1
		rlink.command(MOTOR_3_GO, 0);
		#endif
		return height;
	}
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

		if(readings == WWW) { // slow down if getting close
			approaching = true;
			rotationSpeed = 0;
		}
		else if(readings == BWB) { // on line
			rotationSpeed = 0;
		}
		else if(readings == BBB) { // lost!! disaster recovery		
			throw runtime_error( "robot got lost!" );
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
				throw runtime_error( "robot got lost!" );
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




