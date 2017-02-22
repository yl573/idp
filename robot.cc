
#include "include.h"

using namespace std;

robot::robot() {

	cout << "creating robot" << endl;
	// if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	//   cout << "Cannot initialise link" << endl;
	//   rlink.print_errs("  ");
	// }
}

int robot::moveUntilJunction(int direction) {
	if(direction == FORWARD) {

	}
	else if(direction == BACKWARD) {

	}
	else {
		return ERR_ILLEGAL_VALUE;
	}
	return SUCCESS;
}

int robot::turn(int direction) {
	wheels.setOppositeSpeeds(127);
	while(true);	
	return 0;
}

