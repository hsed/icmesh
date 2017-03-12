//Only definitions
#define DEBUG 0
#define ABOUT "Intelligent Traffic Control System (ITCS) Simulation v0.11" //"Mesh Network Traffic Controller v0.10"
#define WINDOW_HEIGHT 1000
#define WINDOW_WIDTH 1000
#define CAR_SIZE 20.f			//Radius of car
#define TRACK_WIDTH 50.0f
#define PERIOD_S 0.01f			//Sleep time in seconds
#define CAR_SIDES 30Ui64		//Circle = 30Ui64
#define SLOW_MULT 0.5f			//Slow speed mutliplier, 0.1 means make speed 10x slower etc.
#define LANES 8
#define MIN_JUNC_DIST 75 //Tests show 73~75 is the range when enter/exiting junction but this may also be affected by speed steps etc


#define JUNCTIONSIZE 0.1 //Juction Size as percentage of window.

//Max speed of car is 1/4 of JunctionSize* Window_Height
#define TYPICALSPEED 4.0f