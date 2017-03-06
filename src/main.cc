
#include "include.h"
#include "robot.h"
#include "planner.h"

using namespace std;

robot_link rlink;
stopwatch watch;
ofstream myfile;
  

  


int main() {
	if (!rlink.initialise (ROBOT_NUM)) { // setup the link
	  cout << "Cannot initialise link" << endl;
	  rlink.print_errs("  ");
	} else {
		cout << "robot connected" << endl;
	}
	myfile.open ("log.txt");

	//cout << rlink.request (READ_PORT_0) << endl;
	planner myPlanner;
	myPlanner.start();
	myfile.close();
}
