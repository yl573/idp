 
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
		//getToPickUp1(origin);
		//goUpRamp(pickUp2);
		//getToConveyorDrop(pickUp2);
		goToConveyorPickUp(conveyorPickUpJunction);
		pickUpFromConveyor();
		
	} catch(runtime_error& error) {

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
	currentLocation = pickUp1_conveyor;
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
	rbt.setForkliftHeight(5);
	rbt.moveForwardUntilJunction();
	rbt.moveForwardUntilTouch();
	currentLocation = hole;
}

void planner::goUpRamp(location loadLocation) {
	if(loadLocation != pickUp2 && loadLocation != hole)
		throw invalid_argument( "invalid start location to go up the ramp" );
	if(loadLocation == hole){
		rbt.moveBackUntilJunction(); // check if gets the first line
		rbt.turn(RIGHT);
		rbt.turn(RIGHT);
		moveForwardMultipleTimes(2);
		rbt.turn(RIGHT);
	}
	if(loadLocation == pickUp2){
		rbt.moveBackUntilJunction(); // check if gets the first line
		rbt.turn(RIGHT);
	}
	moveForwardMultipleTimes(2);
	rbt.turn(RIGHT);
	moveForwardMultipleTimes(2);
	rbt.turn(RIGHT);
	moveForwardMultipleTimes(4);
	currentLocation = conveyorPickUpJunction;
}

void planner::goToBox1(){
	rbt.moveBackUntilJunction();
	rbt.turn(RIGHT);
	rbt.align();//if needed
	rbt.moveForwardUntilTouch();
	currentLocation = box1;
}

void planner::goToBox2(){
	rbt.moveBackUntilJunction();
	rbt.turn(LEFT);
	rbt.align();//if needed
	rbt.moveForwardUntilTouch();
	currentLocation = box2;
}

void planner::goToConveyorPickUp(location loadLocation){
	if (loadLocation != box1 && loadLocation != box2 && loadLocation != conveyorPickUpJunction){
		throw invalid_argument( "invalid start location to go up the ramp" );
	}
	rbt.setForkliftHeight(0);
	if (loadLocation == conveyorPickUpJunction){
		
	}
	if (loadLocation == box1){
		rbt.moveBackUntilJunction(); //check if moves enough to not touch the pallet
		rbt.turn(RIGHT);
	}
	if (loadLocation == box2){
		rbt.moveBackUntilJunction();
		rbt.turn(LEFT);
	}
	rbt.moveForwardMs(1000);
}

void planner::getToPickUp1(location loadLocation){
	if(loadLocation != pickUp1_conveyor && loadLocation != hole && loadLocation != origin)
		throw invalid_argument( "invalid start location to go to pick up 1" );
	if(loadLocation == pickUp1_conveyor){
		rbt.moveBackUntilJunction();
		rbt.setForkliftHeight(0);
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
	}
	else if(loadLocation == hole){
		rbt.moveBackUntilJunction();
		rbt.turn(RIGHT);
		rbt.moveForwardUntilJunction();
		rbt.setForkliftHeight(0);
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
	}
	else if(loadLocation == origin){
		moveForwardMultipleTimes(3);
		rbt.setForkliftHeight(0);
		rbt.turn(RIGHT);
		rbt.moveForwardUntilTouch();
	}
	loadLocation = pickUp1_conveyor;
}

void planner::getToPickUp2(location loadLocation){
	if(loadLocation != pickUp1_conveyor && loadLocation != hole)
		throw invalid_argument( "invalid start location to go to pick up 2" );
	rbt.moveBackUntilJunction();
	rbt.turn(LEFT);
	delay(200);
	rbt.turn(LEFT);
	rbt.moveForwardUntilJunction();
	if(loadLocation == pickUp1_conveyor){
		rbt.turn(LEFT);
	}
	else if(loadLocation == hole){
		rbt.moveForwardUntilJunction();
	}
	loadLocation = pickUp2;
}

void planner::pickUpFromPickUp(){
	currentColor = rbt.signalLoadType(true);
	if (currentColor == white || currentColor == green || currentColor == red){
		palletColors.push(currentColor);
	}
	rbt.setForkliftHeight(1);
}
void planner::pickUpFromConveyor(){
	currentColor = palletColors.front();
	palletColors.pop(); //check if correct
	rbt.setForkliftHeight(1);
}

void planner::putOnConveyor(){
	rbt.setForkliftHeight(0);
}

void planner::putInHole(){
	rbt.setForkliftHeight(4);
}

void planner::putOnBox1(){
	rbt.setForkliftHeight(0);
	palletOnBox1 = true;
}

void planner::putOnBox2(){
	rbt.setForkliftHeight(0);
	palletOnBox2 = true;
}

void planner::stackOnPallet(){
	rbt.setForkliftHeight(2);
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


