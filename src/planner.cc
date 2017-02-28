
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
		//rbt.moveBackUntilJunction();
		//rbt.moveForwardUntilJunction();
		rbt.turn(LEFT);
	} catch(runtime_error& error) {
		//cout << "ERROR: " << error.what() << endl;
		//rbt.recovery();
	}
}

void dropOnConveyer(location loadLocation) {
	if(loadLocation != pickUp1 || loadLocation != pickUp2)
		throw invalid_argument( "invalid pickup location" );
	rbt.turn(LEFT);
	if(loadLocation == pickUp1) {
		rbt.moveForwardUntilJunction();
	}
}

void dropInHole(location loadLocation) {
	if(loadLocation != pickUp1 || loadLocation != pickUp2)
		throw invalid_argument( "invalid pickup location" );
	rbt.turn(RIGHT);
	if(loadLocation == pickUp2) {
		rbt.moveForwardUntilJunction();
	}
	rbt.turn(RIGHT);
	rbt.moveForwardUntilJunction();	
}

void goUpRamp() {
	if(loadLocation != pickUp2 || loadLocation != dropOffLow)
		throw invalid_argument( "invalid start location to go up the ramp" );
	rbt.turn(LEFT);
	rbt.turn(LEFT);
	rbt.moveForwardUntilJunction();
	if(loadLocation == dropOffLow) 
		rbt.turn(RIGHT);
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilJunction();
	rbt.turn(RIGHT);
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilJunction();
	rbt.turn(RIGHT);
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilJunction();
}



queue<action> planner::getPath(location start, location dest) {
	queue<action> motionPlan;

}
