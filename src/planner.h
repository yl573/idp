#include "include.h"                 
using namespace std;

enum location {origin, pickUp1_conveyor, pickUp2, hole, dropOffHigh};


class planner {

public:
	planner();
	void start();
	

private:
	//location loadLocation;
	void getToConveyorDrop(location loadLocation);
	void getToHole(location loadLocation);
	void goUpRamp(location loadLocation);
	void getToPickUp1(location loadLocation);
	void getToPickUp2(location loadLocation);
	void moveForwardMultipleTimes(int n);

};
