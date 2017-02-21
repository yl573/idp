#include <iostream>
using namespace std;
#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
#include <robot_delay.h>
#define ROBOT_NUM 11   // The id number (see below)
robot_link rlink;

stopwatch watch;

int main ()
{
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	  return -1;
	}

	rlink.command (STOP_IF_HIGH, 0b11111111);
	rlink.command (STOP_IF_LOW, 0b11111111);
	rlink.command (STOP_SELECT, 4);

}