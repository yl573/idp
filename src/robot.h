#include "include.h"
#include "wheels_driver.cc"
#include "forklift_driver.cc"
#include "sensor_reader.cc"

using namespace std;

class robot {
public:
	robot();
	void moveForwardUntilJunction();
	void turn(int direction); 
	void recovery();

private:
	wheelsDriver wheels;
	forkliftDriver forklift;
	sensorReader sensors;
};