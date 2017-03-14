
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
		frontSensorState parsedState = getFrontSensorReading();
		if(parsedState != BBB) 
			cachedState = parsedState;
	}

	// stores reading in board2Reading, bit 1 converts to touching
	void readBoard2() {
		board2Reading = rlink.request (READ_PORT_1);
		touching = (board2Reading & 0b00000010) != 2;
		cout << touching << endl;
		cout << bitset<8>(board2Reading & 0b00100010) << endl;
	}

	// gets bits 2-7 from board2Reading

	int getforkliftReadingsADC(){
		int height0 = rlink.request (ADC0);
		int height2 = rlink.request (ADC1);
		int height4 = rlink.request (ADC2);
		int height5 = rlink.request (ADC3);
		if(height0 > 100 ) { //find the value from tests
			return 0;
		}
		else if(height2 > 100 ) { 
			return 2;
		}
		else if(height4 > 100 ) { 
			return 4;
		}
		else if(height5 > 100 ) { 
			return 5;
		}
		else{
			return -1;
		}
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
	
	// automatically turn on pallet type LEDs
	color checkType() {
		int intensity = rlink.request (ADC4);
		cout << "intensity" << intensity << endl;
		if(intensity < 30 ) {
			return white;
		}
		else if(intensity >= 50 && intensity < 100) {
			return red;
		}
		else if(intensity >= 100 && intensity < 120) {
			return green;
		}
		else if(intensity >= 120) {
			return black;
		}
		else {
			return nothing;
		}
	}

	// flashes for 10 seconds, uses bit 0 from board 2
	void flashNewLoadLed() {
		rlink.command (WRITE_PORT_1, 0b00000001);
		delay (2000);
		rlink.command (WRITE_PORT_1, 0b00000000);
		delay (2000);
		rlink.command (WRITE_PORT_1, 0b00000001);
		delay (2000);
		rlink.command (WRITE_PORT_1, 0b00000000);
		delay (4000);			
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

	void setLeftRight(int l, int r) {
		rlink.command(MOTOR_1_GO, l);
		rlink.command(MOTOR_2_GO, r);
	}
	
	void setStraightRotation(int straight, int rotation) {
		// +ve forward and right, -ve back and left
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
	int mReading;
	forkliftDriver(int h){ lastHeight = h;}

	//set the new height as last height inside setHight(int) or outide?    MOTOR_3 for forklift
	void setHeight(int height){
		sensors.readBoard2();
		mReading = sensors.getForkliftReadingsADC();
		
		if (height == 1){
			if (height > lastHeight){//we are at 0
			#ifndef TEST
			rlink.command(MOTOR_3_GO, 127);
			delay(1000);
			rlink.command (RAMP_TIME, 0);
			rlink.command(MOTOR_3_GO, 0);
			#endif
			}
			if (height < lastHeight){
			#ifndef TEST
			do{
				sensors.readBoard2();
				rlink.command(MOTOR_3_GO, 255);
			}while (getForkliftReadingADC() != 2);
			delay(1000);
			rlink.command (RAMP_TIME, 0);
			rlink.command(MOTOR_3_GO, 0);
			#endif
			}
		}

		else if (height == 3){
			if (height > lastHeight){
			#ifndef TEST
			do{
				sensors.readBoard2();
				rlink.command(MOTOR_3_GO, 127);
			}while (getForkliftReadingADC() != 2);
			delay(1000);
			rlink.command (RAMP_TIME, 0);
			rlink.command(MOTOR_3_GO, 0);
			#endif
			}
			if (height < lastHeight){
			#ifndef TEST
			do{
				sensors.readBoard2();
				rlink.command(MOTOR_3_GO, 255);
			}while (getForkliftReadingADC() != 4);
			delay(1000);
			rlink.command (RAMP_TIME, 0);
			rlink.command(MOTOR_3_GO, 0);
			#endif
			}
		}

		else if (height == lastHeight){
			if(mReading != height){
				while (true){
					rlink.command(MOTOR_3_GO, 127);
					sensors.readBoard2();
					mReading = sensors.getForkliftReadingsADC();
					if(mReading == 0){lastHeight = 0; break;}
					if(mReading == 2){lastHeight = 2; break;}
					if(mReading == 4){lastHeight = 4; break;}
					if(mReading == 5){lastHeight = 5; break;}
				}
				rlink.command (RAMP_TIME, 0);
				rlink.command(MOTOR_3_GO, 0);
			}
		}
		else if (height > lastHeight){
			#ifndef TEST
			do{
				sensors.readBoard2();
				rlink.command(MOTOR_3_GO, 127);
			}while (getForkliftReadingsADC() != height);
			#endif
		}
		else if (height < lastHeight){
			#ifndef TEST
			do{
				readBoard2();
				rlink.command(MOTOR_3_GO, 255);
			}while (getForkliftReadingsADC() != height);
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
	bool backLine = false;
	while(true) {
		sensors.readBoard1();
		frontSensorState reading = sensors.getFrontSensorReading();
		if(reading == WWW) {
			backLine = false;
			cout << "WWW" << endl;
			break;
		}
		else if(sensors.backSensorOnLine) {
			backLine = true;
			cout << "back on line" << endl;
			break;
		}
		wheels.setStraightRotation(lineSpeed, getRotationDemand());
	} 
	if(!backLine) {
		wheels.setStraightRotation(lineSpeed, 0);
		delay(350);
	}
	wheels.brake();
}

void robot::moveForwardUntilTouch(){
	cout << "move forward until touch sensor pressed" << endl;
	int lineSpeed = 60;
	do {
		sensors.readBoard1();
		sensors.readBoard2();
		wheels.setStraightRotation(lineSpeed, 0);//getRotationDemand() * 10);
	} while(!sensors.touching);
	wheels.brake();
}

void robot::moveBackUntilJunction() {
	cout << "move back until junction" << endl;
	int lineSpeed = -70;
	wheels.setStraightRotation(lineSpeed, 0);
	do {
		sensors.readBoard1();
	} while(!sensors.backSensorOnLine);
	wheels.brake();
}

void robot::forkliftUp(int ms) {
	rlink.command(MOTOR_3_GO, 255);
	delay(ms);
	rlink.command(MOTOR_3_GO, 0);
}

void robot::forkliftDown(int ms) {
	rlink.command(MOTOR_3_GO, 127);
	delay(ms);
	rlink.command(MOTOR_3_GO, 0);
}

void robot::test() {
	while(true) {
		/*int reading0 = rlink.request (ADC0);
		int reading1 = rlink.request (ADC1);
		int reading2 = rlink.request (ADC2);
		int reading3 = rlink.request (ADC3);*/
		//if(reading != prev) {
		//reading = reading & 0b00000010;
		sensors.readBoard2();
		//cout << "reading: " << reading0 << " " << reading1 << " " << reading2 << " " << reading3 << "\n";
		//signalLoadType(1);
		//rlink.command(MOTOR_3_GO, 127);
		//rlink.command(MOTOR_4_GO, 255);
	}
} 

void robot::moveBackUntilFrontOnLine() {
	cout << "move back until front on line" << endl;
	int lineSpeed = -127;
	wheels.setStraightRotation(lineSpeed, 0);
	delay(500);
	wheels.brake();
}

int robot::getRotationDemand() { 
	frontSensorState readings = sensors.getFrontSensorReading(); 
	static int integralGain = 0;
	static int count = 0;
	int proportionGain = 40;
	if(readings == BWB || readings == WWW) {
		count = 0;
		integralGain = 0;
		return 0;
	}
	else {
		count++;
		if(count == 10) {
			integralGain++;
			count = 0;
		}
		if(readings == BWW)
			return 1 * proportionGain + integralGain;
		else if(readings == BBW)
			return 2 * proportionGain + integralGain;
		else if(readings == WWB)
			return -1 * proportionGain + integralGain;
		else if(readings == WBB)
			return -2 * proportionGain + integralGain;
		else {
			cout << readings << endl;
			//throw invalid_argument( "front sensor readings wrong, check robot.sensors" );
			return 0;
		}
	}
}

void robot::moveForwardMs(int ms) {
	wheels.setStraightRotation(70, 0);
	delay(ms);
	wheels.brake();
}

void robot::turn(int direction) {
	cout << "turning" << endl;
	int rotationSpeed = 127;

	if(direction != RIGHT && direction != LEFT) 
		throw invalid_argument( "turning direction can only be left or right" );
	if(direction == RIGHT)
		rotationSpeed *= -1;
	
	wheels.setStraightRotation(0, rotationSpeed);
	
	delay(500);
	watch.start();
	do {
		sensors.readBoard1();
	} while(!sensors.backSensorOnLine);
	//delay(80);
	wheels.brake();
	//watch.read() < 1600); //|| 
}

void robot::recovery() {
	cout << "Starting recovery" << endl;
	wheels.brake();
	int rotationSpeed = 64; // if cachedState is not reliable, take a leap of faith
	int lineSpeed = 64;
	if(sensors.cachedState == BWW || sensors.cachedState == BBW) {
		rotationSpeed = 64;
		lineSpeed = 64;
	}
	else if (sensors.cachedState == WWB || sensors.cachedState == WBB) {
		rotationSpeed = -64;
		lineSpeed = -64;
	}

	// S-shaped search
	// An error is throw when a line is found
	try {
		int time = 1000;
		//wheels.setStraightRotation(0, rotationSpeed);
		//waitTimeoutOrReachedLine(time);
		while(true) {
			wheels.setLeftRight(0, 127);
			waitTimeoutOrReachedLine(time);
			wheels.setLeftRight(0, -127);
			waitTimeoutOrReachedLine(time);
			wheels.setLeftRight(127, 0);
			waitTimeoutOrReachedLine(time);
			wheels.setLeftRight(-127, 0);
			waitTimeoutOrReachedLine(time);
			time += 1000;
			// timeouts should make it turn 90 degrees
			//wheels.setStraightRotation(lineSpeed, -rotationSpeed);
			//waitTimeoutOrReachedLine(time);
			
			//wheels.setStraightRotation(127, 0);	
			//waitTimeoutOrReachedLine(500);
					
			//wheels.setStraightRotation(-lineSpeed, rotationSpeed);
			//waitTimeoutOrReachedLine(time*2);
			
			//wheels.setStraightRotation(127, 0);	
			//waitTimeoutOrReachedLine(1000);
			
			//wheels.setStraightRotation(0, -rotationSpeed);
			//waitTimeoutOrReachedLine(time);
			
			//wheels.setStraightRotation(127, 0);	
			//waitTimeoutOrReachedLine(500);
			
			
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

void robot::signalLoadType(bool on) {
	color palletColor = sensors.checkType();
	if (on == true){
		if (palletColor == red){
			rlink.command (WRITE_PORT_0, 0b11010000);
			cout << "red" << endl;
		}
		else if (palletColor == green){
			rlink.command (WRITE_PORT_0, 0b10110000);
			cout << "green" << endl;
		}
		else if (palletColor == black){
			rlink.command (WRITE_PORT_0, 0b01110000);
			cout << "black" << endl;
		}
		else if (palletColor == white){
			rlink.command (WRITE_PORT_0, 0b11100000);
			cout << "white" << endl;
		}
		else {
			rlink.command (WRITE_PORT_0, 0b11110000);
		}
	}
	else {
		rlink.command (WRITE_PORT_0, 0b11110000);
	}
}



