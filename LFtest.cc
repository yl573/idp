#include <iostream>
using namespace std;
#include <robot_instr.h>
#include <robot_link.h>
#include <bitset>


#define ROBOT_NUM 11   // The id number (see below)
robot_link rlink;


int main ()
{
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	  return -1;
	}


	while(true) {
		//rlink.command(MOTOR_3_GO, 127);
		int reading = rlink.command (WRITE_PORT_0, 0b00000000);
		//cout << "reading: " << bitset<8>(reading & 0b00001111) << endl;
	}

}
