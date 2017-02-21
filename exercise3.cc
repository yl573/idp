#include <iostream>
using namespace std;
#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
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

	watch.start();
	for (int i=0; i<1000; ++i){
		rlink.request (TEST_INSTRUCTION); // send test instruction
	}
	int ms = watch.read();
	cout << ms/1000 << endl;

}
