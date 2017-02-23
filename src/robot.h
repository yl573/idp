#include "include.h"


using namespace std;

class robot {
public:
	robot();
	void moveForwardUntilJunction();
	void moveBackUntilJunction();
	void turn(int direction); 
	void recovery();

private:
	wheelsDriver wheels;
	forkliftDriver forklift;
	sensorReader sensors;

	int getOffset(frontSensorState readings);
};
