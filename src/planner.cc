 
#include "include.h"
#include "planner.h"
#include "robot.h"
using namespace std;

robot rbt;

planner::planner() {
	cout << "creating planner" << endl;
}
void planner::start() {
	cout << "mission start" << endl;
	try {
		//rbt.forkliftUp(8000);
		//rbt.test();
		
		//rbt.forkliftDown(10000);
		rbt.setForkliftHeight(0);
		rbt.moveForwardUntilJunction();
		rbt.moveForwardMs(1000);
		rbt.setForkliftHeight(3);
		rbt.moveBackUntilJunction();
		rbt.turn(RIGHT);
		rbt.align();
		rbt.moveForwardUntilTouch();
		rbt.setForkliftHeight(0);
		rbt.moveBackUntilFrontOnLine();
		rbt.setForkliftHeight(3);
		rbt.moveForwardUntilJunction();
		rbt.turn(LEFT);
		rbt.setForkliftHeight(0);
		//rbt.moveForwardUntilJunction();
		//rbt.moveForwardUntilTouch();
		//getToHole(pickUp2);
		//goUpRamp(pickUp2);
		/*rbt.moveForwardUntilJunction();
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
		rbt.moveBackUntilJunction();
		rbt.forkliftDown(10000);
		rbt.moveForwardUntilTouch();
		rbt.forkliftUp(10000);
		rbt.moveBackUntilJunction();
		rbt.turn(LEFT);
		//rbt.moveForwardUntilJunction();
		rbt.moveForwardMs(2000);
		rbt.forkliftDown(10000);
		rbt.moveBackUntilJunction();
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
		rbt.forkliftUp(10000);
		rbt.moveBackUntilJunction();
		rbt.turn(LEFT);
		//rbt.moveForwardUntilJunction();
		
		rbt.forkliftDown(10000);
		rbt.moveBackUntilJunction();

		*/
		/*while(true) {
			rbt.signalLoadType(true);
		}*/
		//rbt.turn(RIGHT);
		//rbt.moveForwardUntilTouch();
	} catch(runtime_error& error) {
		//cout << "ERROR: " << error.what() << endl;
		//rbt.recovery();
	}
}

void planner::moveForwardMultipleTimes(int n){
	for(int i=0; i<n; ++i){
		rbt.moveForwardUntilJunction();
	}
}

void planner::getToConveyorDrop(location loadLocation) {
	if(loadLocation != pickUp1_conveyor && loadLocation != pickUp2)
		throw invalid_argument( "invalid pickup location" );
	rbt.moveBackUntilJunction();	
	rbt.turn(LEFT);
	if(loadLocation == pickUp2) {
		rbt.moveForwardUntilJunction();
	}
	rbt.moveForwardUntilTouch();
}

void planner::getToHole(location loadLocation) {
	if(loadLocation != pickUp1_conveyor && loadLocation != pickUp2)
		throw invalid_argument( "invalid pickup location" );
	rbt.moveBackUntilJunction();
	rbt.turn(RIGHT);
	if(loadLocation == pickUp1_conveyor) {
		rbt.moveForwardUntilJunction();
	}
	rbt.turn(RIGHT);
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilTouch();
}

void planner::goUpRamp(location loadLocation) {
	if(loadLocation != pickUp2 && loadLocation != hole)
		throw invalid_argument( "invalid start location to go up the ramp" );
	//rbt.turn(LEFT);
	//rbt.turn(LEFT);
	rbt.moveForwardUntilJunction();
	if(loadLocation == hole) 
		rbt.turn(RIGHT);
	moveForwardMultipleTimes(2);
	rbt.turn(RIGHT);
	moveForwardMultipleTimes(2);
	rbt.turn(RIGHT);
	moveForwardMultipleTimes(4);
}

void planner::getToPickUp1(location loadLocation){
	if(loadLocation != pickUp1_conveyor && loadLocation != hole && loadLocation != origin)
		throw invalid_argument( "invalid start location to go up the ramp" );
	if(loadLocation == pickUp1_conveyor){
		rbt.moveBackUntilJunction();
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
	}
	else if(loadLocation == hole){
		rbt.moveBackUntilJunction();
		rbt.turn(RIGHT);
		rbt.moveForwardUntilJunction();
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
	}
	else if(loadLocation == origin){
		rbt.turn(RIGHT); //check how we start
		moveForwardMultipleTimes(3);
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
	}
}

void planner::getToPickUp2(location loadLocation){
	if(loadLocation != pickUp1_conveyor && loadLocation != hole)
		throw invalid_argument( "invalid start location to go up the ramp" );
	rbt.moveBackUntilJunction();
	rbt.turn(LEFT);
	rbt.turn(LEFT);
	rbt.moveForwardUntilJunction();
	if(loadLocation == pickUp1_conveyor){
		rbt.turn(LEFT);
	}
	else if(loadLocation == hole){
		rbt.moveForwardUntilJunction();
	}
}


/*
void planner::stackPallet(int platform, int height) {
	rbt.moveBackUntilJunction();
	if(platform == 1)
		rbt.turn(LEFT);
	else
		rbt.turn(RIGHT);
	rbt.moveForwardUntilTouch();
}

void planner::returnToTopConveyer(int platform) {
	rbt.moveBackUntilJunction();
	rbt.moveBackUntilFrontOnLine();
	//raise forklift
	rbt.moveForwardUntilJunction();
	if(platform == 1)
		rbt.turn(RIGHT);
	else
		rbt.turn(LEFT);
	rbt.moveForwardUntilTouch();
}*/


