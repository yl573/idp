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

	int ms;

	watch.start();
	for (int i=0; i<100; ++i){
		rlink.request (READ_PORT_0);
	}
	ms = watch.read();
	cout << ms/100 << "ms" << endl;

	watch.start();
	for (int i=0; i<100; ++i){
		rlink.command (WRITE_PORT_0, 0b11111111);
	}
	ms = watch.read();
	cout << ms/100 << "ms" << endl;

	watch.start();
	for (int i=0; i<100; ++i){
		rlink.request (ADC0);
	}
	ms = watch.read();
	cout << ms/100 << "ms" << endl;

	watch.start();
	for (int i=0; i<100; ++i){
		rlink.command(BOTH_MOTORS_GO_OPPOSITE, 0b01111111);
	}
	ms = watch.read();
	cout << ms/100 << "ms" << endl;

}