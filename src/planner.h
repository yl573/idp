#include "include.h"                 
using namespace std;

enum location {origin, pickUp1_conveyor, pickUp2, hole, box1, box2, conveyorPickUpJunction};


class planner {

public:
	location currentLocation;
	color currentColor;
	queue<color> palletColors;
	planner();
	void start();
	

private:
	//location loadLocation;
	void getToConveyorDrop(location loadLocation);
	void getToHole(location loadLocation);
	void goUpRamp(location loadLocation);
	void goToBox1();// done/ from conveyor pick up
	void goToBox2();// done/ from conveyor pick up
	void goToConveyorPickUp(location loadLocation);// done/ from goUpRamp or Box1 or Box2
	void getToPickUp1(location loadLocation);
	void getToPickUp2(location loadLocation);
	void moveForwardMultipleTimes(int n);
	void pickUpFromPickUp();// done/
	void pickUpFromConveyor();// done/
	void putOnConveyor();//
	void putInHole();//
	void putOnBox();//


};
