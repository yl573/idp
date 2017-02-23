
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
		rbt.moveForwardUntilJunction();
		//rbt.turn(LEFT);
	} catch(runtime_error& error) {
		cout << "ERROR: " << error.what() << endl;
		rbt.recovery();
	}
}
