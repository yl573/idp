#include "include.h"
#define STARTING_POSITION 0
using namespace std;

enum frontSensorState {BWB, WWB, WBB, BWW, BBW, BBB, WWW};
enum color {red, green, black, white};

class robot {
public:
	robot();
	void moveForwardUntilJunction();
	void moveBackUntilJunction();
	void turn(int direction); 
	void moveBackUntilFrontOnLine();
	void recovery();
private:
	int getOffset(frontSensorState readings);
	void waitTimeoutOrReachedLine(int timeout);
	int getRotationDemand();
};
