#include <iostream>
using namespace std;
#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
#include <robot_delay.h>
#define ROBOT_NUM 11   // The id number (see below)
robot_link rlink;
stopwatch watch;

inline int getSpeed(int speed, bool reverse = false) {
	return reverse? speed + 128 : speed;
}

int main ()
{
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	  return -1;
	}

	rlink.command(BOTH_MOTORS_GO_SAME, getSpeed(127));

	rlink.command (RAMP_TIME, 127);

}

