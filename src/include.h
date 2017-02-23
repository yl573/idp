#ifndef INCLUDED
#define INCLUDED
//#define TEST

#include <iostream>
#include <vector>
#include <stdexcept>

#ifndef TEST
#include <robot_instr.h>
#include <robot_link.h>
#include <stopwatch.h>
extern robot_link rlink;
extern stopwatch watch;
#endif

#define ROBOT_NUM 11   // The id number (see below)

#define FORWARD 1
#define BACKWARD 2
#define LEFT 3
#define RIGHT 4
// #define
// #define
// #define
// #define


using namespace std;

#endif
