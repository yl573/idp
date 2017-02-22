
#include "../include/include.h"
using namespace std;

planner::planner() {
	cout << "creating planner" << endl;
}

void planner::start() {
	//rbt.moveUntilJunction(FORWARD);
	rbt.turn(LEFT);
}
