#include "include.h"
#define STARTING_POSITION 0
using namespace std;

enum frontSensorState {BWB, WWB, WBB, BWW, BBW, BBB, WWW, WBW};
enum color {red, green, black, white};
enum turned {on, off};

class robot {
public:

	robot();
	void moveForwardUntilJunction();
	void moveForwardUntilTouch();
	void moveBackUntilJunction();
	void turn(int direction); 
	void moveBackUntilFrontOnLine();
	void recovery();
	void test();
	void signalLoadType();
private:
	int getOffset(frontSensorState readings);
	void waitTimeoutOrReachedLine(int timeout);
	int getRotationDemand();
};
