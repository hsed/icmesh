#pragma once

#include "entity.h"
#include "constants.h"
#include "lane.h"
#include <vector>
#include <algorithm>

using namespace sf;

class Car : public Entity {

public:
	//DEFAULT INITIALISER
	Car(sf::Color color = sf::Color::Red, float radius = CAR_RADIUS, size_t sides = CAR_SIDES, sf::Vector2f position = sf::Vector2f(0, 0), bool centeredOrigin = true) {
		//Default Constructor as all arguments are optional
		this->setFillColor(color);
		this->setRadius(radius);
		this->setPointCount(sides);
		if (centeredOrigin) { this->setOrigin(this->getRadius(), this->getRadius()); } //adjust origin before setting position
		this->setPosition(position);
		this->setOutlineColor(Color(255, 255, 255));
		this->setOutlineThickness(-1);
		command = CommandType::Ready;
		currentCommand = CommandType::Ready;

		currentState = { -1, Lane::Undefined, Lane::Undefined, 0, -1, getDistFromJunc(), -1, false }; //need to implement speed and position from intersection
	}

	//EACH CAR'S DATA PACKET
	struct DataPacket {
		int carID;
		Lane::LaneType laneID;
		Lane::LaneType intendedLaneID;
		int ticksWaited; //Priority availiable to other cars
		float speed;
		//Wasn't included: float direction; //Direction is included for when we implement this in hardware. As we need to use the raw direction to calculate these "other properties"
		float relDist; //relative distance from intersection == dist from centre of window (assume google maps)
		float prevTime; //temp fix for issue with bouding boxes and intersection
		bool atJunc; //is the car currently crossing the junction
	};

	//Overloaded func to also update dist when pos is set
	void setPosition(float x, float y) {
		sf::CircleShape::setPosition(x, y);
		updateDistFromJunc();
	}

	void setPosition(Vector2f position) {
		sf::CircleShape::setPosition(position);
		updateDistFromJunc();
	}

	void update() {
		Entity::update(); //important
		updateDistFromJunc();
	}

	void setVelocity(sf::Vector2f velocity) {
		Entity::setVelocity(velocity); //important
		this->currentState.speed = ((velocity.x == 0) ? velocity.y : velocity.x); //spped has no direction so need to set x or y or either if both zero
	}

	bool checkCollision(sf::Shape* entity) {
		bool isIntersect = Entity::checkCollision(entity);
		if (isIntersect) {
			currentState.atJunc = true;
		}
		else {
			currentState.atJunc = false;
		}
		return isIntersect;
	}

	void updateDistFromJunc() {
		this->currentState.relDist = getDistFromJunc();
	}

	int setPriority(std::vector<Car::DataPacket> carsInLane, bool frontOfLane, int noOfTimesWaited) //returns an int for compatibility with mbed (for scalable sizing)
	{
		//CALCULATIONS TO SEE WHETHER FRONT OF LANE OR NOT
		if (frontOfLane)
		{
			return 0;
		}

		int priority = 1;
		priority = priority + currentState.speed; //May need scaling
		priority = priority + carsInLane.size();
		priority = priority + (noOfTimesWaited)*(noOfTimesWaited); //Grows at a faster rate than the previous function
		priority = priority * (currentState.laneID)*(currentState.intendedLaneID) *2; //Intended lane direction
																				   //BELOW IMPLEMENT TRUTH TABLE TO FIND OUT IF THE CAR IS COMPATIBLE WITH ANY OTHER FRONT CAR. IF SO DOUBLE ITS PRIORITY
		if (currentState.laneID == Lane::LaneType::LaneEI)
		{

		}
		else if (currentState.laneID == Lane::LaneType::LaneSI)
		{

		}
		else if (currentState.laneID == Lane::LaneType::LaneWI)
		{

		}
		else if (currentState.laneID == Lane::LaneType::LaneNI)
		{

		}
		return priority;
	} //Note that the carList must be updated to use the priority to set the speeds etc. of the front cars. The speeds of cars behind the front car must
	  //be calculated automatically, or an algorithm must be implemented to calculate their speeds according to number of cars in the lane vs other lanes
	  //(i.e. assumption is that the car will go in the order of its priority - in this case probably need to calculate separate priority for the cars behind
	  //the front car in each lane)

#pragma region Commands
	enum CommandType {
		Stop,	//Hazard
		Slow,	//Caution
		Ready
	};

	CommandType getCommand() { return command; }

	CommandType setCommand(CommandType command) {
		if (command == CommandType::Stop && this->currentCommand != CommandType::Stop) {
			this->stop();
		}
		if (command == CommandType::Slow && this->currentCommand != CommandType::Slow) {
			this->slow();
		}
		if (command == CommandType::Ready && this->currentCommand != CommandType::Ready) {
			this->restart();
		}
		return this->command = command;
	}
	//Keep this function for now

	void recievePackets(std::vector<Car::DataPacket> allPackets) {
		using namespace std;

		vector<std::vector<Car::DataPacket>> packetsPerLane;
		vector<Car::DataPacket> tmpLanePackets;

		for (int i = 0; i < LANES; i++) { packetsPerLane.push_back(tmpLanePackets); }
		for (int i = 0; i < allPackets.size(); i++) {
			if (allPackets[i].laneID >= 0 && allPackets[i].laneID < LANES) {
				packetsPerLane[allPackets[i].laneID].push_back(allPackets[i]);
			}
			else {
				std::cout << "ERROR: Cannot get packet from car with id " << allPackets[i].laneID << "as laneID is invalid." << std::endl;
			}
		}

		for (int i = 0; i < packetsPerLane.size(); i++) {
			sort(packetsPerLane[i].begin(), packetsPerLane[i].end(), compRelDist);
		}

		//Least time to cross => crosses first algorithm.
		vector<int> prioritiesPerLane;
		for (int i = 0; i < packetsPerLane.size(); i++) {
			prioritiesPerLane.push_back(setPriority(packetsPerLane[i], false, this->currentState.ticksWaited));
		}


		Lane::LaneTime temp;
		//TimeID temp;
		std::vector<Lane::LaneTime> leastTimesToEnter;



		//ALL ODD LANES_IDs are INWARD LANES -- ONLY ODD LANES HERE
		for (int i = 1; i < LANES; i += 2) {
			if (!packetsPerLane[i].empty() ) {
				temp.laneID = packetsPerLane[i][0].laneID;	//assume topmost is nearest
															//temp.timeToEnter = abs((packetsPerLane[i][0].relDist +(TRACK_WIDTH*2)) / packetsPerLane[i][0].speed) * PERIOD_S; //assume topmost is nearest
				temp.time = abs((packetsPerLane[i][0].relDist) / packetsPerLane[i][0].speed) * PERIOD_S; //this version only calcs time to junc
				leastTimesToEnter.push_back(temp); //only get nearest to junc per lane
			}
		}



		if (!leastTimesToEnter.empty()) {
			sort(leastTimesToEnter.begin(), leastTimesToEnter.end(), Lane::compLaneTime);
			for (int i = 0; i < leastTimesToEnter.size(); i++) {
				if (DEBUG) cout << "\nNearest car in lane: " << Lane::laneString(leastTimesToEnter[i].laneID) << " will take: " << leastTimesToEnter[i].time << "s to enter junction." << endl;
			}
		}

		bool isAtJunc = false;
		bool skipRest;
		for (int i = 0; i < allPackets.size() && isAtJunc == false; i++) {
			if (allPackets[i].atJunc) {
				//inside to minus from junc width
				float relDist = abs(allPackets[i].relDist);
				float timeToCross = abs((((TRACK_WIDTH * 2) + CAR_RADIUS * 2) - relDist) / allPackets[i].speed) * PERIOD_S;
				if (DEBUG) cout << "Car in junction" << " with ID: " << allPackets[i].carID << " will take: " << timeToCross << "s to cross" << endl;
				skipRest = false;

				for (int j = 0; j < leastTimesToEnter.size() && !skipRest; j++) {
					if (leastTimesToEnter[j].time > (timeToCross)) {
						//skipRest = true; //after the first value > timetocross all other must be > cause sorted, give +0.5sec buffer
					}
					else {
						if (DEBUG) cout << "\nTiming may be an issue for car in lane: " << Lane::laneString(leastTimesToEnter[j].laneID) << endl;
						//same or less to enter as it is to cross

						Lane::LaneType self = allPackets[i].laneID; //original laneid of crosser
						Lane::LaneType nearby = packetsPerLane[leastTimesToEnter[j].laneID][0].laneID; //get nearest node in that lane, assume topmost
						Lane::LaneType selfIntent = allPackets[i].intendedLaneID;
						Lane::LaneType nearbyIntent = packetsPerLane[leastTimesToEnter[j].laneID][0].intendedLaneID; //get nearest node in that lane, assume topmost
						float distToJunc = packetsPerLane[leastTimesToEnter[j].laneID][0].relDist;
						if (DEBUG) cout << "My intended lane: " << Lane::laneString(selfIntent) << "; Issue car intended lane: " << Lane::laneString(nearbyIntent) << endl;

						if (Lane::canCrash(self, nearby, selfIntent, nearbyIntent) || !isHP(prioritiesPerLane)) {
							if (DEBUG) cout << "Crash is viable slowing down all cars in lane: " << leastTimesToEnter[j].laneID << endl;
							


							if (distToJunc < 15) {
								if (this->getPacket().laneID == leastTimesToEnter[j].laneID && !this->getPacket().atJunc) {
										this->setCommand(Car::Stop);
										++this->currentState.ticksWaited;
								}
							}
							else if (distToJunc < 50) {
								if (this->getPacket().laneID == leastTimesToEnter[j].laneID && !this->getPacket().atJunc) {
										//this->setCommand(Car::Slow);
								}
							}
							else {
								//this->setCommand(Car::Ready);
							}
						}
					}
				}
			}
		}

		//Get every car with time to enter at junc < time to cross
		//for each of these u need intended lane ids, if they math or special case 7-1, 3-5 then tel car to slow down

	}
#pragma endregion

	Color getColor() { return this->getFillColor(); }

	void setID(int ID) { currentState.carID = ID; }

	int getID() { return currentState.carID; }

	void setLaneID(Lane::LaneType ID) { currentState.laneID = ID; }

	void setIntLaneID(Lane::LaneType ID) { currentState.intendedLaneID = ID; }

	void cross() {
		this->currentState.laneID = this->currentState.intendedLaneID;
	}

	void setPrevTime(float time) {
		this->currentState.prevTime = time;
	}

	void resetWaitTimes() {
		this->currentState.ticksWaited = 0;
	}
	//Get LaneID from datapacket

	DataPacket getPacket() { return this->currentState; }

	static sf::Color getRandColor(){
		return sf::Color((rand() % 51) * 5, (rand() % 51) * 5, (rand() % 51) * 5);
	}

private:

	//Internal Variables
	//int ticksWaited;

	DataPacket currentState;

	CommandType command;		//old
	CommandType currentCommand; //old

	float getDistFromJunc() {
		Vector2f juncTL(WINDOW_WIDTH / 2 - TRACK_WIDTH, WINDOW_HEIGHT / 2 - TRACK_WIDTH); // Top-Left Corner of junction
		float x = this->getPosition().x;
		float y = this->getPosition().y;

		switch (this->currentState.laneID) {
		case 0:
			y += CAR_RADIUS;
			return juncTL.y - y;
		case 1:
			y += CAR_RADIUS;
			return juncTL.y - y;
		case 2:
			x -= CAR_RADIUS;
			return x - (juncTL.x + 2 * TRACK_WIDTH);
		case 3:
			x -= CAR_RADIUS;
			return x - (juncTL.x + 2 * TRACK_WIDTH);
		case 4:
			y -= CAR_RADIUS;
			return y - (juncTL.y + 2 * TRACK_WIDTH);
		case 5:
			y -= CAR_RADIUS;
			return y - (juncTL.y + 2 * TRACK_WIDTH);
		case 6:
			x += CAR_RADIUS;
			return juncTL.x - x;
		case 7:
			x += CAR_RADIUS;
			return juncTL.x - x;
		}

		////not a very good idea to do pythagoras maybe bounds is better
		//float x = (WINDOW_WIDTH / 2) - ;
		//float y = (WINDOW_HEIGHT / 2) - this->getPosition().y;
		////std::cout << "x: " << x << " y: " << y << " sqrt: " << sqrt(pow(x, 2) + pow(y, 2)) << std::endl; //debugging only
		//return sqrt(pow(x, 2) + pow(y, 2));
	}

	//For commands only atm
	void stop() {
		this->velocity = Vector2f(0, 0);
		this->currentCommand = CommandType::Stop;
	}

	void slow() {
		if (this->currentCommand == CommandType::Stop) {
			this->velocity = SLOW_MULT * this->initVelocity;
		}
		else if (this->currentCommand == CommandType::Ready) {
			this->velocity = SLOW_MULT * this->velocity;
		}
		this->currentCommand = CommandType::Slow;
	}

	void restart() {
		this->velocity = this->initVelocity;
		this->currentCommand = CommandType::Ready;
	}

	bool isHP(std::vector<int>& prioritiesPerLane) {
		//assume lane starts from 0-->7
		Lane::LaneType selfLane = this->currentState.laneID;
		Lane::LaneType maxScoreLane = Lane::Undefined;
		int max = 0;
		for (int i = 0; i < prioritiesPerLane.size(); i++) {
			if (prioritiesPerLane[i] > max) {
				max = prioritiesPerLane[i];
				maxScoreLane = (Lane::LaneType)i;
			}
		}

		if (this->currentState.laneID == maxScoreLane) {
			return true;
		}
		else {
			//if undef or 0 score or not max score lane
			return false;
		}
	}

	static bool compRelDist(DataPacket i, DataPacket j) { return (i.relDist < j.relDist); }
};