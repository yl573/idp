
#include "include.h"
#include "robot.h"

using namespace std;

// calibrate sensors
class sensorReader {
public:

	frontSensorState cachedState;
	bool touching;

	void readBoard1() {
		board1Reading = rlink.request (READ_PORT_0);
		frontSensorState parsedState= getFrontSensorReading();
		if(parsedState != BBB) 
			cachedState = parsedState;
	}

	void readBoard2() {
		board2Reading = rlink.request (READ_PORT_1);
		touching = (bool) board2Reading & 0b00000010;
	}

	int getForkliftReadings() {	
		int forkliftReading = board2Reading & 0b00111111;

		// if one and only one bit is 1
		if (forkliftReading && !(forkliftReading & (forkliftReading-1)))
		{
			int mask = 0b00000001;
			int position;
			for(position = 0; position < 5; position++) {
				if(mask & forkliftReading) // that bit is 1
					return position;
				// if not shift the mask
				mask <<= 1;
			}
		}
		else
			return -1;
	}
	
	color checkType() {
		int intensity = rlink.request (ADC0)
		if(intensity < 50 ) {
			rlink.command (WRITE_PORT_0, 0b00010000);
			return white;
		}
		if(intensity > 50 && intensity < 100) {
			rlink.command (WRITE_PORT_0, 0b00100000);
			return green;
		}
		if(intensity > 50 && intensity < 150) {
			rlink.command (WRITE_PORT_0, 0b01000000);
			return red;
		}
		else {
			rlink.command (WRITE_PORT_0, 0b10000000);
			return black;
		}
	}

	void changeNewLoadLed(bool on) {
		if(on)
			rlink.command (WRITE_PORT_1, 0b00000001);
		else
			rlink.command (WRITE_PORT_1, 0b00000000);
	}

	// get readings from the front sensors
	frontSensorState getFrontSensorReading() {
		bool print = true;
		int frontReadings = board1Reading & 0b00000111;
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
		return board1Reading & 00001000;
	}

	int getColorReading(int &color) {
		return 0;
	}

private:
	int board1Reading;
	int board2Reading;
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
	sensors.readBoard1();
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
	watch.start(); 
	while(watch.read() < 10000){
		sensors.readBoard1();
	}

	frontSensorState currentState;
	watch.start();
	while(true) {
		rotationSpeed = 127;
		sensors.readBoard1();
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
	if(sensors.cachedState == BWW || sensors.cachedState == BBW) {
		rotationSpeed = 64;
	}
	else if (sensors.cachedState == WWB || sensors.cachedState == WBB) {
		rotationSpeed = -64;
	}

	// S-shaped search
	// An error is throw when a line is found
	try {
		int time = 1000;
		while(true) {
			// timeouts should make it turn 90 degrees
			wheels.setStraightRotation(0, rotationSpeed);
			waitTimeoutOrReachedLine(time);
			wheels.setStraightRotation(0, -rotationSpeed);
			waitTimeoutOrReachedLine(time);
			time += 500;
		}
	} catch(runtime_error& error) {
		wheels.brake();
		cout << error.what() << endl;
	}
}

// throws exception when a line is found
void robot::waitTimeoutOrReachedLine(int timeout) {
	watch.start();
	while(watch.read() < timeout) { 
		sensors.readBoard1();
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




