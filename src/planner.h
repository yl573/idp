#include "include.h"                 
using namespace std;

enum location {origin, pickUp1_conveyor, pickUp2, hole, box1, box2};


class planner {

public:
	planner();
	void start();
	

private:
	//location loadLocation;
	void getToConveyorDrop(location loadLocation);
	void getToHole(location loadLocation);
	void goUpRamp(location loadLocation);
	void goToBox1();//from conveyor pick up
	void goToBox2();//from conveyor pick up
	void goToConveyorPickUp(location loadLocation);//from goUpRamp or Box1 or Box2
	void getToPickUp1(location loadLocation);
	void getToPickUp2(location loadLocation);
	void moveForwardMultipleTimes(int n);
	void pickUpFromPickUp();//
	void pickUpFromConveyor();//
	void putOnConveyor();//
	void putInHole();//
	void putOnBox();//


};
