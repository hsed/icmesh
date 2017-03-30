#pragma once
#if defined (_MSC_VER)  // Visual studio
#define thread_local __declspec( thread )
#elif defined (__GCC__) // GCC
#define thread_local __thread
#endif

#include <random>
#include <time.h>
#include <thread>
#include <SFML/Graphics.hpp>
//#include <random>

using namespace sf;

//int operator + (const Lane::LaneType & Op1, const Lane::LaneType & Op2);

class Lane : public sf::RectangleShape {
public:
	enum LaneType {
		Undefined = -1, //For initialisation
		LaneNO = 0, //North Outward
		LaneNI = 1, //North Inward
		LaneEO = 2, //East Outward
		LaneEI = 3,
		LaneSO = 4,
		LaneSI = 5,
		LaneWO = 6,
		LaneWI = 7
	};

	Lane() {
		//default, needed for lane
		this->laneID = Undefined;
		//srand((unsigned int)time(NULL));

	}
	Lane(LaneType ID, Vector2f size, Vector2f position, Color color = sf::Color::White) {
		this->laneID = ID;
		this->setSize(size);
		this->setPosition(position);
		this->setFillColor(color);
		//this->setOrigin(TRACK_WIDTH / 2, 0); //top-centre for all lanes <-- this is buggy for now
	}
	~Lane() {}

	LaneType getLaneID() { return laneID; }
	//LaneID only set when constructing object.

	struct LaneTime {
		LaneType laneID;
		float time;
		int ticksWaited;
	};

	static std::string laneString(Lane::LaneType ID) {
		switch (ID) {
		case 0: return "North-Out";
		case 1: return "North-In";
		case 2: return "East-Out";
		case 3: return "East-In";
		case 4: return "South-Out";
		case 5: return "South-In";
		case 6: return "West-Out";
		case 7: return "West-In";
		}
	}

	static bool canCrash(Lane::LaneType ID1Cur, Lane::LaneType ID2Cur, Lane::LaneType ID1Fut, Lane::LaneType ID2Fut) {
		if (ID1Fut == ID2Fut) {
			return true;	//Same intended lane, so can definitely crash
		}
		else if ((ID1Cur % 2 == 1) && (ID2Cur % 2 == 1) && (abs(ID2Cur - ID1Cur) == 2 || abs(ID2Cur - ID1Cur) == 6)) {
			//both cars originate from odd lanes (inward) and the lane difference is 2 OR 6 so 7-1,1-7,3-5,5-3,5-7,7-5,1-3,3-1
			if (DEBUG) std::cout << "CAN CRASH ODD & 2 DIFF" << std::endl;
			return true;
		}
		else {
			return false; //default case
		}
	}

	enum TurningDirection
	{
		Left = 3,
		Forward = 2,
		Right = 1,
		UndefinedDirection = -1
	};

	friend TurningDirection operator + (const Lane::LaneType & Op1, const Lane::LaneType & Op2) //The spirit of the function - returns turning direction by adding the lanes
	{
		int x, y;

		if (Op1 == Lane::LaneType::LaneEI)
		{
			x = 10;
		}
		else if (Op1 == Lane::LaneType::LaneSI)
		{
			x = 20;
		}
		else if (Op1 == Lane::LaneType::LaneWI)
		{
			x = 30;
		}
		else if (Op1 == Lane::LaneType::LaneNI)
		{
			x = 40;
		}
		else
		{
			return Lane::TurningDirection::UndefinedDirection;
		}

		if (Op2 == Lane::LaneType::LaneEO)
		{
			y = 1;
		}
		else if (Op2 == Lane::LaneType::LaneSO)
		{
			y = 2;
		}
		else if (Op2 == Lane::LaneType::LaneWO)
		{
			y = 3;
		}
		else if (Op2 == Lane::LaneType::LaneNO)
		{
			y = 4;
		}
		else
		{
			return Lane::TurningDirection::UndefinedDirection;
		}

		int z = x + y;

		switch (z)
		{
		case(14):
			return Lane::TurningDirection::Right;
			break;
		case(13):
			return Lane::TurningDirection::Forward;
			break;
		case(12):
			return Lane::TurningDirection::Left;
			break;
		case(21):
			return Lane::TurningDirection::Right;
			break;
		case(24):
			return Lane::TurningDirection::Forward;
			break;
		case(23):
			return Lane::TurningDirection::Left;
			break;
		case(32):
			return Lane::TurningDirection::Right;
			break;
		case(31):
			return Lane::TurningDirection::Forward;
			break;
		case(34):
			return Lane::TurningDirection::Left;
			break;
		case(43):
			return Lane::TurningDirection::Right;
			break;
		case(42):
			return Lane::TurningDirection::Forward;
			break;
		case(41):
			return Lane::TurningDirection::Left;
			break;
		default:
			return Lane::TurningDirection::UndefinedDirection;
		}
	}

	static bool compLaneTime(LaneTime i, LaneTime j) { return (i.time < j.time); }

	static Lane::LaneType getRandLane(LaneType startLane = Undefined) {
		//if start is undefined then we return a starting lane, otherwise we return an intended lane
		if (startLane == Lane::LaneType::Undefined) {
			return (Lane::LaneType)(((rand() % 4) * 2) + 1);
		}
		else {
			switch (startLane) {
			case LaneNI:
				return LaneSO;
			case LaneSI:
				return LaneNO;
			case LaneEI:
				return LaneWO;
			case LaneWI:
				return LaneEO;
			}
		}
	}

	static Vector2f getRandInPos(LaneType laneType) {
		//get a random position in any of the inward lanes only.
		//std::uniform_int_distribution<> dis(1, MAX_CARS_IN_LANE);
		float multiplier = intRand(1, MAX_CARS_IN_LANE);//int((rand() % MAX_CARS_IN_LANE) + 1); //imagine 10 cars, should this be the 1st..2nd.......or..10th car

		float maxDist = -(TRACK_WIDTH + 3 * CAR_RADIUS); //eq is maxwidth = laneLength - trackW + 3* carRadius //laneLength calc later
		//float safeDist = CAR_RADIUS * 3; //the safe distance interval which has only 1 car present == atleast 1.5 cars
		float intervalDist;
		Vector2f vec;
		int max = MAX_CARS_IN_LANE;
		switch (laneType) {
		case LaneNI:
			//Top Edge -> Middle


			maxDist += (WINDOW_HEIGHT / 2); //the missing factor in equation
			intervalDist = maxDist / MAX_CARS_IN_LANE;

			vec = Vector2f((WINDOW_WIDTH + TRACK_WIDTH) / 2, intervalDist * multiplier);
			if (DEBUG) std::cout << "intDist: " << intervalDist << "LANENI: " << vec.x << " " << vec.y << " mult " << multiplier << std::endl;
			return Vector2f((WINDOW_WIDTH + TRACK_WIDTH) / 2, intervalDist * multiplier);
		case LaneSI:
			//Bottom Edge -> Middle
			maxDist += WINDOW_HEIGHT / 2;
			intervalDist = maxDist / MAX_CARS_IN_LANE;
			return Vector2f((WINDOW_WIDTH - TRACK_WIDTH) / 2, (WINDOW_HEIGHT / 2) + TRACK_WIDTH + intervalDist * multiplier);
			break;
		case LaneEI:
			//Right Edge -> Middle
			maxDist += WINDOW_WIDTH / 2;
			intervalDist = maxDist / MAX_CARS_IN_LANE;
			return Vector2f((WINDOW_WIDTH / 2) + TRACK_WIDTH + intervalDist * multiplier, (WINDOW_HEIGHT + TRACK_WIDTH) / 2);
		case LaneWI:
			//Left Edge -> Middle
			maxDist += WINDOW_WIDTH / 2;
			intervalDist = maxDist / MAX_CARS_IN_LANE;
			return Vector2f(intervalDist * multiplier, (WINDOW_HEIGHT - TRACK_WIDTH) / 2);
		default:
			if (DEBUG) std::cout << "Incorrect LaneID for RandPos" << std::endl;
			return Vector2f(0, 0); //if not inward then set car to infinity.
		}



	}

	static Vector2f getRandVel(LaneType laneType) {
		//from stackoverflow
		float speed = floatRand(MIN_CAR_SPEED, MAX_CAR_SPEED);
			

		switch (laneType) {
		case LaneType::LaneNO:
			return Vector2f(0, -speed);
		case LaneNI:
			return Vector2f(0, speed);
			break;
		case LaneEO:
			return Vector2f(speed, 0);
		case LaneEI:
			return Vector2f(-speed, 0);
		case LaneSO:
			return Vector2f(0, speed);
		case LaneSI:
			return Vector2f(0, -speed);
		case LaneWO:
			return Vector2f(-speed, 0);
		case LaneWI:
			return Vector2f(speed, 0);
		};
	}

	static float getMaxLaneLength(LaneType laneType) {
		//returns the maxDist from edge of junction to farthest edge of lane.
		float maxDist = -(TRACK_WIDTH); //eq is maxwidth = laneLength - trackW + 3* carRadius //laneLength calc later
		if (laneType == 0 || laneType == 1 || laneType == 4 || laneType == 5) {
			//vertical tracks
			maxDist += (WINDOW_HEIGHT / 2);
		}
		else {
			//all others assume horizontal
			maxDist += (WINDOW_WIDTH / 2);
		}
		return maxDist;
	}

	/* Thread-safe function that returns a random number between min and max (inclusive).
	This function takes ~142% the time that calling rand() would take. For this extra
	cost you get a better uniform distribution and thread-safety. */
	static int intRand(const int & min, const int & max) {
		static std::mt19937* generator = NULL;
		if (!generator) generator = new std::mt19937(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
		std::uniform_int_distribution<> distribution(min, max);
		return distribution(*generator);
	}

	static float floatRand(const int & min, const int & max) {
		static std::mt19937* generator = NULL;
		if (!generator) generator = new std::mt19937(clock() + std::hash<std::thread::id>()(std::this_thread::get_id()));
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(*generator);
	}

private:
	LaneType laneID;

};
