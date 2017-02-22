#pragma once
#include "include.h"
#include "wheels_driver.cc"
#include "forklift_driver.cc"
#include "sensor_reader.cc"
using namespace std;

class robot {
public:
	robot();
	int moveUntilJunction(int direction);
	int turn(int direction); 

private:
	wheelsDriver wheels;
	forkliftDriver forklift;
	sensorReader sensors;

};