#include <iostream>
using namespace std;
#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
#define ROBOT_NUM 11   // The id number (see below)
robot_link rlink;      // datatype for the robot link

int main ()
{
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	  return -1;
	}


	const int gobit = 127;
	const int stopbit = 0;
	

	
	//rlink.command(MOTOR_2_GO, gobit);
	rlink.command(MOTOR_3_GO, gobit);
	
	while(true);


}
/*
void pause(timems) {
	stopwatch watch;
	watch.start();
	while(watch.read() < timems)
	return;
}*/
