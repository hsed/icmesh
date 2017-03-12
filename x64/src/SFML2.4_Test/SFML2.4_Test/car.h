#pragma once

#include "entity.h"
#include "constants.h"
#include "lane.h"
#include <vector>
#include <algorithm>

using namespace sf;
using namespace std;

class Car : public Entity {

public:
	Car(sf::Color color = sf::Color::Red, float radius = CAR_SIZE, size_t sides = CAR_SIDES, sf::Vector2f position = sf::Vector2f(0, 0), bool centeredOrigin = true);

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

		if (distFromJunction < 0&&hasTurned==false) 
			switch (turningDirection){
			{
			case Lane::TurningDirection::Left:
			{
				float absoluteJunctionSize = JUNCTIONSIZE*WINDOW_HEIGHT;
				if (distFromJunction > -((absoluteJunctionSize / 4) + (absoluteJunctionSize / 8)) && distFromJunction < -((absoluteJunctionSize / 4) - (absoluteJunctionSize / 8))) {
					hasTurned = true;
					Vector2f newVelocity = Lane::getLaneSpeed(this->currentState.intendedLaneID);
					this->currentState.laneID = this->currentState.intendedLaneID;
					this->setVelocity(newVelocity);
				}
				break;
			}
			case Lane::TurningDirection::Right:
				break;
			case Lane::TurningDirection::Forward:
				break;
			default:
				throw 100;
			}





		}
		//TODO: This assumes that the window is square.
		else if ((distFromJunction > WINDOW_WIDTH / 2&&distFromJunction!=INFINITY)||this->queuedForRefresh) {
			this->ResetCar(*laneList);
		}
	}

	void setVelocity(sf::Vector2f velocity) {
		Entity::setVelocity(velocity); //important
		this->currentState.speed = ((velocity.x == 0) ? velocity.y : velocity.x); //spped has no direction so need to set x or y or either if both zero
	}

	void ResetCar(vector<Lane>& laneList, Lane::LaneType startLane=Lane::LaneType::Undefined, Lane::LaneType destLane=Lane::LaneType::Undefined) {

		this->laneList = &laneList;

		startLane = startLane==Lane::LaneType::Undefined?(Lane::LaneType)(((rand() % 4) * 2) + 1):startLane;
		//Pause Untill Lane is empty
		while (laneList[startLane].getMilliSecondsTillRegenerateCar() > 0) {
			this->queuedForRefresh = true;
			return;
		}



		
		destLane = destLane == Lane::LaneType::Undefined ? (Lane::LaneType)(((rand() % 4) * 2) ) : destLane;
		//remove potential fro u turn;
		if ((destLane / 2) == (startLane / 2)) {

			int rand3 = rand() % 3;
			if (rand3 == startLane / 2) {
				rand3 += 1;
				rand3 = rand3 % 3;
			}
				destLane = (Lane::LaneType)(rand3*2);

		}
		int r = (rand() % 51) * 5;
		int g = (rand() % 51) * 5;
		int b = (rand() % 51) * 5;
		this->setFillColor(Color(r, g, b));
		this->setPosition(laneList[startLane].getPosition());
		//this->setPosition(500, 500);
		
		this->setVelocity(Lane::getLaneSpeed(startLane));
		this->setLaneID(startLane);
		this->setIntLaneID(destLane);
		this->hasTurned = false;
		this->updateDistFromJunc();
		turningDirection = calcualteTurnDirection(startLane, destLane);

		this->queuedForRefresh = false;

		laneList[startLane].LastGeneratatedTime.restart();
		int tb = laneList[startLane].LastGeneratatedTime.getElapsedTime().asMilliseconds();
	}

	Lane::TurningDirection calcualteTurnDirection(Lane::LaneType start, Lane::LaneType end) {
		int diff = abs(start - end) % 6;
		return (Lane::TurningDirection)(diff / 2);
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
		this->currentState.relDist = calculateDistFromJunc();
		distFromJunction = calculateDistFromJunc();
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

	void recievePackets(std::vector<Car::DataPacket> allPackets) {
		using namespace std;

		vector<std::vector<Car::DataPacket>> packetsPerLane;
		vector<Car::DataPacket> tmpLanePackets;

		for (int i = 0; i < LANES; i++) { 
			packetsPerLane.push_back(tmpLanePackets); 
		}


		for (int i = 0; i < allPackets.size(); i++) {
			if (allPackets[i].laneID >= 0 && allPackets[i].laneID < LANES) {
				packetsPerLane[allPackets[i].laneID].push_back(allPackets[i]);
			}
			else {
				std::cout << "ERROR: Cannot get packet from car with id " << allPackets[i].laneID << "as laneID is invalid." << std::endl;
			}
		}



		//Least time to cross => crosses first algorithm.


		Lane::LaneTime temp;
		//TimeID temp;
		std::vector<Lane::LaneTime> leastTimesToEnter;



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
				float timeToCross = abs((((TRACK_WIDTH * 2) + CAR_SIZE * 2) - relDist) / allPackets[i].speed) * PERIOD_S;
				if (DEBUG) cout << "Car in junction" << " with ID: " << allPackets[i].carID << " will take: " << timeToCross << "s to cross" << endl;
				skipRest = false;
				//if (timeToCross < allPackets[i].prevTime || allPackets[i].prevTime == -1) { //this has been fixed by adding virtual padding to junction
					//only test if time is NOT increasing
					//cout << "still on prev: " << allPackets[i].prevTime << " now: " << timeToCross << endl;
					//allPackets[i].prevTime = timeToCross;
					//getCarByID(allPackets[i].carID)->setPrevTime(timeToCross);

				for (int j = 0; j < leastTimesToEnter.size() && !skipRest; j++) {
					if (leastTimesToEnter[j].time > (timeToCross + 0.5)) {
						skipRest = true; //after the first value > timetocross all other must be > cause sorted, give +0.5sec buffer
					}
					else {
						if (DEBUG) cout << "\nTiming may be an issue for car in lane: " << Lane::laneString(leastTimesToEnter[j].laneID) << endl;
						//same or less to enter as it is to cross

						Lane::LaneType self = allPackets[i].laneID; //original laneid of crosser
						Lane::LaneType nearby = packetsPerLane[leastTimesToEnter[j].laneID][0].laneID; //get nearest node in that lane, assume topmost
						Lane::LaneType selfIntent = allPackets[i].intendedLaneID;
						Lane::LaneType nearbyIntent = packetsPerLane[leastTimesToEnter[j].laneID][0].intendedLaneID; //get nearest node in that lane, assume topmost

						if (DEBUG) cout << "My intended lane: " << Lane::laneString(selfIntent) << "; Issue car intended lane: " << Lane::laneString(nearbyIntent) << endl;

						if (Lane::canCrash(self, nearby, selfIntent, nearbyIntent)) {
							if (DEBUG) cout << "Crash is viabe slowing down all cars in lane: " << leastTimesToEnter[j].laneID << endl;
							for (int k = 0; k < packetsPerLane[leastTimesToEnter[j].laneID].size(); k++) {
								int ID = packetsPerLane[leastTimesToEnter[j].laneID][k].carID;
								if (ID == this->getID() && !this->getPacket().atJunc) {
									if (this->getCommand() == Car::Ready) {
										this->setCommand(Car::Slow); //needs more testing to see if it actually slows first
									}
									else if (this->getCommand() == Car::Slow) {
										//If slow but still can crash then just stop
										this->setCommand(Car::Stop);
									}
										//a very ineff way atm need to make it better aka reduce no of loops
									//Stop is better for now cause u r guaranteed no crash!
								}
								//Car* temp = getCarByID(ID);
								//if (temp != NULL) {
									//temp->setCommand(Car::Slow);
								//}
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

	void setLaneID(Lane::LaneType ID) { 
		currentState.laneID = ID;
	}

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
	float distFromJunction;
	CommandType command;		//old
	CommandType currentCommand; //old
	bool hasTurned = false;
	vector<Lane>* laneList;
	Lane::TurningDirection turningDirection;
	bool queuedForRefresh = false;

	float calculateDistFromJunc() {
		
		float x = this->getPosition().x;
		float y = this->getPosition().y;
		
		int direction = this->currentState.laneID / 2;

		
		if (direction == 0 ||direction==2) {
			return abs((WINDOW_HEIGHT / 2) - y) - ((JUNCTIONSIZE*WINDOW_HEIGHT)/2);
		}
		else if(direction ==3 ||direction ==1) {
			return abs((WINDOW_WIDTH / 2) - x) - ((JUNCTIONSIZE*WINDOW_WIDTH) / 2);
		}
		else {
			return INFINITY;
		}
		
	}

	//Gets a random start time based on the time since car last created.



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