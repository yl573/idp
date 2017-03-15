#include "include.h"
#define STARTING_POSITION 0
using namespace std;

enum frontSensorState {BWB, WWB, WBB, BWW, BBW, BBB, WWW, WBW};


class robot {
public:
	robot();
	void moveForwardUntilJunction();
	void flashNewLoadLed();
	void moveForwardUntilTouch();
	void moveBackUntilJunction();
	void turn(int direction); 
	void moveBackUntilFrontOnLine();
	void recovery();
	void test();
	void align();
	color signalLoadType(bool on);
	void forkliftUp(int ms);
	void forkliftDown(int ms);
	void moveForwardMs(int ms);
	void setForkliftHeight(int height);
private:
	int getOffset(frontSensorState readings);
	void waitTimeoutOrReachedLine(int timeout);
	int getRotationDemand();
};
