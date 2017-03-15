#ifndef INCLUDED
#define INCLUDED

#define TEST


#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <stdexcept>

#include <robot_instr.h>
#include <robot_link.h>
#include <robot_delay.h>
#include <stopwatch.h>
#include <bitset>

using namespace std;


enum color {red, green, black, white, nothing};

extern robot_link rlink;
extern stopwatch watch;
extern ofstream myfile;

#define ROBOT_NUM 11   // The id number (see below)

#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4
// #define
// #define
// #define
// #define
#define STARTING_POSITION 0

using namespace std;

#endif
