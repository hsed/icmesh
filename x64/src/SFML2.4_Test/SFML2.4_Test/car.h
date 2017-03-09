#pragma once

#include "entity.h"
#include "constants.h"
#include "lane.h"
#include <vector>
#include <algorithm>

using namespace sf;

class Car : public Entity {

public:
	Car(sf::Color color = sf::Color::Red, float radius = CAR_SIZE, size_t sides = CAR_SIDES, sf::Vector2f position = sf::Vector2f(0, 0), bool centeredOrigin = true) {
		//Default Constructor as all arguments are optional
		this->setFillColor(color);
		this->setRadius(radius);
		this->setPointCount(sides);
		if (centeredOrigin) { this->setOrigin(this->getRadius(), this->getRadius()); } //adjust origin before setting position
		this->setPosition(position);

		command = CommandType::Ready;
		currentCommand = CommandType::Ready;

		currentState = {-1, Lane::Undefined, Lane::Undefined, -1, getDistFromJunc(), -1, false}; //need to implement speed and position from intersection

	}

	//Overloaded func to also update dist when pos is set
	void setPosition(float x, float y) {
		sf::CircleShape::setPosition(x, y);
		updateDistFromJunc();
	}

	void setPosition(Vector2f position) {
		sf::CircleShape::setPosition(position);
		updateDistFromJunc();
	}

	struct DataPacket {
		int carID;
		Lane::LaneType laneID;
		Lane::LaneType intendedLaneID;
		float speed; //direction is implied by other properties so this is Speed in X or Y
//		float direction; //Direction is included for when we implement this in hardware. As we need to use the raw direction to calculate these "other properties"
		float relDist; //relative distance from intersection == dist from centre of window (assume google maps)
		float prevTime; //temp fix for issue with bouding boxes and intersection
		bool atJunc; //is the car currently crossing the junction
	};

	void update() {
		Entity::update(); //important
		updateDistFromJunc();
	}

	void setVelocity(sf::Vector2f velocity) {
		Entity::setVelocity(velocity); //important
		this->currentState.speed = ((velocity.x == 0) ? velocity.y: velocity.x); //spped has no direction so need to set x or y or either if both zero
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

	void recieveCommands(std::vector<Car::CommandType> cmds) {
		//cannt use std as namespace atm, using sf?
		int cmdScore = 1; //default score
		for (int i = 0; i < cmds.size(); i++) {
			//assume that that evry id car is in ascending order
			//std::cout << "The car with ID " << i << " has command: " << cmds[i] << std::endl;
			cmdScore *= cmds[i];
		}
		double maxScore = pow((int)Car::CommandType::Ready, cmds.size()); //if all cars are fine the score == max_score == Ready^(car.size)
		double cmdProb = cmdScore / maxScore;
		//std::cout << "The probablity of stopping is: " << (1 - cmdProb) << std::endl;
		/*
		if (cmdProb == 0 && this->currentCommand != CommandType::Stop) {
			std::cout << "ID: " << this->getID() << "\tDECISION: STOP!!" << "\t(Probability: " << cmdProb << ")" << std::endl;
			this->stop();
		}
		else if (cmdProb == 1 && this->currentCommand != CommandType::Ready) {
			std::cout << "ID: " << this->getID() << "\tDECISION: Continue" << "\t(Probability: " << cmdProb << ")" << std::endl;
			this->restart(); //if slow or stopped travel at random speed.
		}
		else if (cmdProb > 0 && cmdProb < 1 && this->currentCommand != CommandType::Slow) {
			//For ready state 0 < cmdProb < 1
			//but this can be adjusted to allow for more iterim states
			std::cout << "ID: " << this->getID() << "\tDECISION: SLOW" << "\t(Probability: " << cmdProb << ")" << std::endl;
			this->slow(); //if slow or stopped travel at random speed.
		}
		else {
			//continue in the current state
		}
		*/
	}

	void recievePackets(vector<Car::DataPacket> allPackets) {
		vector<vector<Car::DataPacket>> packetsPerLane;
		vector<Car::DataPacket> tmpLanePackets;

		for (int i = 0; i < LANES; i++) { packetsPerLane.push_back(tmpLanePackets); }
		for (int i = 0; i < allPackets.size(); i++) {
			if (allPackets[i].laneID >= 0 && allPackets[i].laneID < LANES) {
				packetsPerLane[allPackets[i].laneID].push_back(allPackets[i]);
			}
			else {
				cout << "ERROR: Cannot get packet from car with id " << allPackets[i].laneID << "as laneID is invalid." << endl;
			}
		}

		

		//Least time to cross => crosses first algorithm.


		Lane::LaneTime temp;
		//TimeID temp;
		vector<Lane::LaneTime> leastTimesToEnter;



		//ALL ODD LANES_IDs are INWARD LANES -- ONLY ODD LANES HERE
		for (int i = 1; i < LANES; i += 2) {
			if (!packetsPerLane[i].empty() && !packetsPerLane[i][0].atJunc) {
				temp.laneID = packetsPerLane[i][0].laneID;	//assume topmost is nearest
															//temp.timeToEnter = abs((packetsPerLane[i][0].relDist +(TRACK_WIDTH*2)) / packetsPerLane[i][0].speed) * PERIOD_S; //assume topmost is nearest
				temp.time = abs((packetsPerLane[i][0].relDist) / packetsPerLane[i][0].speed) * PERIOD_S; //this version only calcs time to junc
				leastTimesToEnter.push_back(temp); //only get nearest to junc per lane
			}
		}



		if (!leastTimesToEnter.empty()) {
			std::sort(leastTimesToEnter.begin(), leastTimesToEnter.end(), CarList::compTimeID);
			for (int i = 0; i < leastTimesToEnter.size(); i++) {
				cout << "Nearest car in lane: " << Lane::laneString(leastTimesToEnter[i].laneID) << " will take: " << leastTimesToEnter[i].time << "s to enter junction." << endl;
			}
		}

		bool isAtJunc = false;
		bool skipRest;
		for (int i = 0; i < allPackets.size() && isAtJunc == false; i++) {
			if (allPackets[i].atJunc) {
				//inside to minus from junc width
				float relDist = abs(allPackets[i].relDist);
				float timeToCross = abs((((TRACK_WIDTH * 2) + CAR_SIZE * 2) - relDist) / allPackets[i].speed) * PERIOD_S;
				cout << "Car in junction will take: " << timeToCross << "s to cross" << endl;
				skipRest = false;
				//if (timeToCross < allPackets[i].prevTime || allPackets[i].prevTime == -1) { //this has been fixed by adding virtual padding to junction
				if (true) {
					//only test if time is NOT increasing
					cout << "still on prev: " << allPackets[i].prevTime << " now: " << timeToCross << endl;
					//allPackets[i].prevTime = timeToCross;
					//getCarByID(allPackets[i].carID)->setPrevTime(timeToCross);

					for (int j = 0; j < leastTimesToEnter.size() && !skipRest; j++) {
						if (leastTimesToEnter[j].time >(timeToCross + 0.5)) {
							skipRest = true; //after the first value > timetocross all other must be > cause sorted, give +0.5sec buffer
						}
						else {
							cout << "time maybe an issue" << endl;
							//same or less to enter as it is to cross

							Lane::LaneType self = allPackets[i].laneID;
							Lane::LaneType nearby = packetsPerLane[leastTimesToEnter[j].laneID][0].laneID; //get nearest node in that lane, assume topmost
							Lane::LaneType selfIntent = allPackets[i].intendedLaneID;
							Lane::LaneType nearbyIntent = packetsPerLane[leastTimesToEnter[j].laneID][0].intendedLaneID; //get nearest node in that lane, assume topmost

							cout << "my id: " << Lane::laneString(selfIntent) << " other id: " << Lane::laneString(nearbyIntent) << endl;

							if (Lane::canCrash(self, nearby, selfIntent, nearbyIntent)) {
								cout << "Crash is viabe slowing down all cars in lane: " << leastTimesToEnter[j].laneID << endl;
								for (int k = 0; k < packetsPerLane[leastTimesToEnter[j].laneID].size(); k++) {
									int ID = packetsPerLane[leastTimesToEnter[j].laneID][k].carID;
									//Car* temp = getCarByID(ID);
									//if (temp != NULL) {
										//temp->setCommand(Car::Slow);
									//}
								}
							}
						}
					}
				}
				//isAtJunc = true;
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
	//Get LaneID from datapacket

	DataPacket getPacket() { return this->currentState; }

private:
	DataPacket currentState;

	CommandType command;		//old
	CommandType currentCommand; //old

	float getDistFromJunc() {
		Vector2f juncTL(WINDOW_WIDTH / 2 - TRACK_WIDTH, WINDOW_HEIGHT / 2 - TRACK_WIDTH); // Top-Left Corner of junction
		float x = this->getPosition().x;
		float y = this->getPosition().y;

		switch (this->currentState.laneID) {
		case 0:
			y += CAR_SIZE;
			return juncTL.y - y;
		case 1:
			y += CAR_SIZE;
			return juncTL.y - y;
		case 2:
			x -= CAR_SIZE;
			return x - (juncTL.x + 2 * TRACK_WIDTH);
		case 3:
			x -= CAR_SIZE;
			return x - (juncTL.x + 2 * TRACK_WIDTH);
		case 4:
			y -= CAR_SIZE;
			return y - (juncTL.y + 2 * TRACK_WIDTH);
		case 5:
			y -= CAR_SIZE;
			return y - (juncTL.y + 2 * TRACK_WIDTH);
		case 6:
			x += CAR_SIZE;
			return juncTL.x - x;
		case 7:
			x += CAR_SIZE;
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


};