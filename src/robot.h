#include "include.h"

using namespace std;

enum frontSensorState {BWB, WWB, WBB, BWW, BBW, BBB, WWW};

class robot {
public:
	robot();
	void moveForwardUntilJunction();
	void moveBackUntilJunction();
	void turn(int direction); 
	void recovery();
private:
	int getOffset(frontSensorState readings);
	void waitTimeoutOrReachedLine(int timeout);
};
