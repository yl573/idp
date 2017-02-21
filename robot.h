#include <iostream>
using namespace std;
#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
#define ROBOT_NUM 11   // The id number (see below)
robot_link rlink;
stopwatch watch;

class robot {
public:
	int moveUntilJunction(bool forward);
	int turn(int direction); 

private:

	class sensorReader {
	public:
		int getLineFollowingReading(int &s1, int &s2, int &s3, nt &s4);
		int getWeightReading(int &weight);
		int getColorReading(int &color);
	} sensors;

	class forkliftDriver {
	public:
		int setHeight(int height);
	} forklift;

	class wheelsDriver {
	public:
		int setSpeeds(int leftSpeed, int rightSpeed);
		int brake();
	} wheels;

};