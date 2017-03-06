#include "include.h"          
#include <queue>        
using namespace std;

enum location {origin, pickUp1, pickUp2, dropOffLow, dropOffHigh};


class planner {

public:
	planner();
	void start();
	

private:
	location loadLocation;
	void getToConveyorDrop(location loadLocation);
	void getToHole(location loadLocation);
	void goUpRamp(location loadLocation);
	void moveForwardMultipleTimes(int n);
};
