
#include "include.h"
#include "robot.h"

using namespace std;

// calibrate sensors
class sensorReader {
public:

	frontSensorState cachedState;
	bool touching;
	int frontReadings;
	bool backSensorOnLine;

	// stores reading in board1Reading, bits 0-3 converts to cached state
	void readBoard1() {
		 int newReading = rlink.request (READ_PORT_0) & 0b00001111;
		 if(newReading != board1Reading) {
			cout << bitset<8>(newReading)<< endl;
			board1Reading = newReading;
			frontReadings = board1Reading & 0b00000111;
		}
		backSensorOnLine = board1Reading & 0b00001000;
		//cout << bitset<8>(board1Reading) << "  " << bitset<8>(board1Reading bitand 00001000) << endl;
		//cout << backSensorOnLine << endl;
		frontSensorState parsedState = getFrontSensorReading();
		if(parsedState != BBB) 
			cachedState = parsedState;
	}

	// stores reading in board2Reading, bit 1 converts to touching
	void readBoard2() {
		board2Reading = rlink.request (READ_PORT_1);
		touching = (bool) board2Reading & 0b00000010;
	}

	// gets bits 2-7 from board2Reading
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
	
	// automatically turn on pallet type LEDs
	color checkType() {
		int intensity = rlink.request (ADC0);
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

	// flashes for 10 seconds, uses bit 0 from board 2
	void flashNewLoadLed() {
		rlink.command (WRITE_PORT_1, 0b00000001);
		delay (2000);
		rlink.command (WRITE_PORT_1, 0b00000000);
		delay (8000);
		rlink.command (WRITE_PORT_1, 0b00000001);
		delay (2000);
		rlink.command (WRITE_PORT_1, 0b00000000);			
	}

	// get readings from the front sensors
	frontSensorState getFrontSensorReading() {
		int frontReadings = board1Reading & 0b00000111;
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
				return WBW;
		}
	}

private:
	int board1Reading;
	int board2Reading;
} sensors;

class wheelsDriver {
public:


	// +ve forward and right, -ve back and left
	void setStraightRotation(int straight, int rotation) {
		//cout << "straight" << straight << "  rotation" << rotation << endl;
		int leftDemand = clamp(straight + rotation);
		int rightDemand = clamp(-straight + rotation);
		leftDemand = convert(leftDemand);
		rightDemand = convert(rightDemand);
		//cout << "leftDemand" << leftDemand << "  rightDemand" << rightDemand << endl;
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
	int lastHeight;//reference
	int currentHeight; //could read all the sensors to know what's the current height but it's wasteful
	bool aligned = false;
	int mReading;
	forkliftDriver(int h){ lastHeight = h;}

	//set the new height as last height inside setHight(int) or outide?    MOTOR_3 for forklift
	void setHeight(int height){
		sensors.readBoard2();
		mReading = sensors.getForkliftReadings();
		if (height == lastHeight){
			if(mReading != height){
				while (true){
					rlink.command(MOTOR_3_GO, 127);
					sensors.readBoard2();
					mReading = sensors.getForkliftReadings();
					if(mReading == 0){lastHeight = 0; break;}
					if(mReading == 1){lastHeight = 1; break;}
					if(mReading == 2){lastHeight = 2; break;}
					if(mReading == 3){lastHeight = 3; break;}
					if(mReading == 4){lastHeight = 4; break;}
					if(mReading == 5){lastHeight = 5; break;}
				}
			}
		}
		if (height > lastHeight){
			#ifndef TEST
			do{
				sensors.readBoard2();
				rlink.command(MOTOR_3_GO, 127);
			}while (getForkliftReadings() != height);
			#endif
		}
		if (height < lastHeight){
			#ifndef TEST
			do{
				readBoard2();
				rlink.command(MOTOR_3_GO, 255);
			}while (getForkliftReadings() != height);
			#endif
		}
		#ifndef TEST
		rlink.command (RAMP_TIME, 0);//quick stopping, have to stop while the sensor still reads 1
		rlink.command(MOTOR_3_GO, 0);
		#endif
		lastHeight = height;
	}
} forklift(STARTING_POSITION); //startingPosition is global; indicates the starting position of the forklift



robot::robot() {

	cout << "creating robot" << endl;
}

void robot::moveForwardUntilJunction() {
	cout << "move forward until junction" << endl;
	int lineSpeed = 127;
	wheels.setStraightRotation(lineSpeed, 0);
	delay(1000);
	do {
		sensors.readBoard1();
		cout << sensors.backSensorOnLine << endl;
		wheels.setStraightRotation(lineSpeed, getRotationDemand());
	} while(!sensors.backSensorOnLine);
	cout << "braking  " << "back sensor online " << sensors.backSensorOnLine << endl;
	wheels.brake();
}

void robot::moveForwardUntilTouch(){
	cout << "move forward until touch sensor pressed" << endl;
	int lineSpeed = 127;
	wheels.setStraightRotation(lineSpeed, 0);
	delay(1000);
	do {
		sensors.readBoard2();
		wheels.setStraightRotation(lineSpeed, getRotationDemand());
	} while(!sensors.touching);
	cout << "braking  " << "touching " << sensors.touching << endl;
	wheels.brake();
}

void robot::moveBackUntilJunction() {
	cout << "move back until junction" << endl;
	int lineSpeed = -127;
	wheels.setStraightRotation(lineSpeed, 0);
	do {
		sensors.readBoard1();
		//wheels.setStraightRotation(lineSpeed, -getRotationDemand());
	} while(!sensors.backSensorOnLine);
	wheels.brake();
}

void robot::test() {
	int prev;
	watch.start();
	while(true) {
		int reading = rlink.request (READ_PORT_0);
		if(reading != prev) {
			cout << "board1: " << bitset<8>(reading) << "\n";
			prev = reading;
		}
	}
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
	frontSensorState readings = sensors.getFrontSensorReading(); 
	cout << bitset<8>(sensors.frontReadings) << endl;
	if(readings == BWB || readings == WWW) 
		return 0;
	else if(readings == BBB) {
		// lost!! disaster recovery
		recovery();
		return 0;
	}
	else {
		// just adjust path
		int offset = getOffset(readings);
		return offset * 80
		
		;
	}
}


void robot::turn(int direction) {
	cout << "turning" << endl;
	int rotationSpeed = 127;

	if(direction != RIGHT && direction != LEFT) 
		throw invalid_argument( "turning direction can only be left or right" );

	if(direction == RIGHT)
		rotationSpeed *= -1;
	
	wheels.setStraightRotation(0, rotationSpeed);
	
	delay(1800);

	/*watch.start();
	while(true) {
		
		sensors.readBoard1();
		//currentState = sensors.getFrontSensorReading();
		//cout << "readings: " << bitset<8>(sensors.frontReadings) << endl;

		if(sensors.frontReadings == 0b00000010) {//sensors.frontReadings != 0b0000000 || sensors.frontReadings != 0b00000101) {
			cout << "turn complete" << endl;
			break;
		}
		else {
			if(watch.read()> 10000)
				recovery();
		}
		
		wheels.setStraightRotation(0, rotationSpeed);
	}*/

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
			time += 1000;
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
		//throw invalid_argument( "front sensor readings wrong, check robot.sensors" );
		return 0;
	}
}




