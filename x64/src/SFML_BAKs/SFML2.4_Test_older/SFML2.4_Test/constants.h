//Only definitions
#define DEBUG 0
#define ABOUT "Mesh Network Traffic Controller v0.10"
#define WINDOW_HEIGHT 480
#define WINDOW_WIDTH 640
#define CAR_SIZE 20.f			//Radius of car
#define TRACK_WIDTH 50.f
#define PERIOD_S 0.01f			//Sleep time in seconds
#define CAR_SIDES 30Ui64		//Circle = 30Ui64
#define SLOW_MULT 0.5f			//Slow speed mutliplier, 0.1 means make speed 10x slower etc.
#define LANES 8
#define MIN_JUNC_DIST 75		//Tests show 73~75 is the range when enter/exiting junction but this may also be affected by speed steps etc