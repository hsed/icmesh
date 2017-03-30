//Only definitions
#define DEBUG 0
#define ABOUT "Intelligent Traffic Control System (ITCS) Simulation v0.15" //"Mesh Network Traffic Controller v0.10"
#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280
#define CAR_RADIUS 20.f			//Radius of car
#define TRACK_WIDTH 50.f
#define PERIOD_S 0.0001f		//Sleep time in seconds
#define CAR_SIDES 30Ui64		//Circle = 30Ui64
#define SLOW_MULT 0.1f			//Slow speed mutliplier, 0.1 means make speed 10x slower etc.
#define LANES 8
#define MIN_JUNC_DIST 75		//Tests show 73~75 is the range when enter/exiting junction but this may also be affected by speed steps etc
#define MIN_DIST_TO_JUNC 10		//only stop when near junc
#define MAX_CARS_IN_LANE (int)((WINDOW_HEIGHT/2)/(CAR_RADIUS*4)) //ATLEAST LEAVE 2 CAR GAP, currently equates to 4
#define MAX_CAR_SPEED 1.f//2.f//1.f
#define MIN_CAR_SPEED 0.2f//1.5f//0.2f
#define MAX_CARS 10//14//15 //total no of cars