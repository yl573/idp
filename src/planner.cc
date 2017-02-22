
#include "include.h"
using namespace std;

planner::planner() {
	cout << "creating planner" << endl;
}

void planner::start() {
	try {
		rbt.moveForwardUntilJunction();
		rbt.turn(LEFT);
	} catch(runtime_error& error) {
		cout << "ERROR: " << error.what() << endl;
		rbt.recovery();
	}
}
