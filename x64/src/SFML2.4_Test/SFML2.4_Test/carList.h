#pragma once
#include "linkedList.h"
#include "car.h"


class CarList : public LinkedList<Car> {
	//This class inherits LinkedList
public:
	CarList() {
		isStarted = false;
		carIndex = 0;
	}
	~CarList() {

	}

	void addItem(Lane::LaneType laneID, Lane::LaneType intendedLaneID = Lane::Undefined, sf::Color color = sf::Color::Red, sf::Vector2f position = sf::Vector2f(0, 0), sf::Vector2f velocity = sf::Vector2f(0, 0)) {
		Car* tempCar = new Car(color); //custom car
		tempCar->setPosition(position);
		tempCar->setVelocity(velocity);
		tempCar->setID(carIndex++); //This must be called first cause object is passed by val NOT REF on LinkedList::addItem
		tempCar->setLaneID(laneID);
		tempCar->setIntLaneID(intendedLaneID);
		LinkedList::addItem(*tempCar); //call default command, you dont need virtual for some reason??? (i dont really understand them well)
		cout << "Created car with index: " << tempCar->getID() << ", colour: (" << +tempCar->getColor().r << ", " << +tempCar->getColor().g << ", " << +tempCar->getColor().b << ")";
		cout << ", laneID: " << tempCar->getPacket().laneID << " and intendedLaneID: " << tempCar->getPacket().intendedLaneID << endl;
		delete tempCar; //free up memory!
	}

	bool getAnimStat() {
		return isStarted;
	}
	void setAnimStat(bool start) {
		isStarted = start;
	}


	void drawList(sf::RenderWindow& windowHandle) {
		if (mainList != NULL) {
			listPtr tempPtr = mainList; //point to head first
			drawList(windowHandle, tempPtr);
		}

	}

	//MAIN PROCESSING MODULE
	void processCommands() {
		cout << endl << endl;
		//vector<Car::DataPacket> packets = new vector<Car::DataPacket>[LANES]; 
		vector<vector<Car::DataPacket>> packetsPerLane;
		vector<Car::DataPacket> tmpLanePackets, allPackets;

		//An array of vector containing the DataPacket from every car (this gives current status)
		//Each array element is for one lane only.

		//leave this for now
#pragma region Commands
		vector<Car::CommandType> cmds; //vector list of commands
		//gather the commands from all cars in a vector, same as mesh network -> TX
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			cmds.push_back(tempPtr->val.getCommand());
		}

		//push all gathered commands to evry car, same as mesh network -> RX
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			tempPtr->val.recieveCommands(cmds);
		}
#pragma endregion

		//TODO: Do Stuff here @sdpenquin @mohika
		//processising stuff

		for (int i = 0; i < LANES; i++) { packetsPerLane.push_back(tmpLanePackets); }
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			allPackets.push_back(tempPtr->val.getPacket()); //needed later

			if (tempPtr->val.getPacket().laneID >= 0 && tempPtr->val.getPacket().laneID <= LANES) {
				packetsPerLane[tempPtr->val.getPacket().laneID].push_back(tempPtr->val.getPacket());
			}
			else {
				cout << "ERROR: Cannot get packet from car with id " << tempPtr->val.getPacket().carID << "as laneID is invalid." << endl;
			}

		}

		//Least time to cross => crosses first algorithm.



		TimeID temp;
		vector<TimeID> leastTimesToEnter;



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
				float timeToCross = abs((((TRACK_WIDTH * 2) + CAR_SIZE*2) - relDist) / allPackets[i].speed) * PERIOD_S;
				cout << "Car in junction will take: " << timeToCross << "s to cross" << endl;
				skipRest = false;
				//if (timeToCross < allPackets[i].prevTime || allPackets[i].prevTime == -1) { //this has been fixed by adding virtual padding to junction
				if (true) {
					//only test if time is NOT increasing
					cout << "still on prev: " << allPackets[i].prevTime  << " now: " << timeToCross << endl;
					//allPackets[i].prevTime = timeToCross;
					getCarByID(allPackets[i].carID)->setPrevTime(timeToCross);

					for (int j = 0; j < leastTimesToEnter.size() && !skipRest; j++) {
						if (leastTimesToEnter[j].time > (timeToCross + 0.5)) {
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
									Car* temp = getCarByID(ID);
									if (temp != NULL) {
										temp->setCommand(Car::Slow);
									}
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

	bool setCommand(int ID, Car::CommandType cmd) {
		//Set a command for a particular car given by ID
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (tempPtr->val.getID() == ID) {
				tempPtr->val.setCommand(cmd);
				return true;
			}
		}
		return false;
	}

	void checkPositions(sf::RenderWindow& windowHandle) {
		//imp! bounds are top-left corner origin but cars themselves are centered origin
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (tempPtr->val.getGlobalBounds().left > windowHandle.getSize().x) {
				//reset position to WI
				tempPtr->val.setPosition(0 - CAR_SIZE, tempPtr->val.getPosition().y);
				tempPtr->val.setLaneID(Lane::LaneWI);
			}
			else if (tempPtr->val.getGlobalBounds().left < 0 - CAR_SIZE * 2) {
				//reset position to EI
				tempPtr->val.setPosition(windowHandle.getSize().x + CAR_SIZE, tempPtr->val.getPosition().y);
				tempPtr->val.setLaneID(Lane::LaneEI);
			}
			else if (tempPtr->val.getGlobalBounds().top > windowHandle.getSize().y) {
				//reset position (else, since no diagonal movement) to NI
				tempPtr->val.setPosition(tempPtr->val.getPosition().x, 0 - CAR_SIZE);
				tempPtr->val.setLaneID(Lane::LaneNI);
			}
			else if (tempPtr->val.getGlobalBounds().top < 0 - CAR_SIZE * 2) {
				//reset position to SI
				tempPtr->val.setPosition(tempPtr->val.getPosition().x, windowHandle.getSize().y + CAR_SIZE);
				tempPtr->val.setLaneID(Lane::LaneSI);
			}
		}

	}

	void checkJunction(Lane& junction) {
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (!tempPtr->val.getPacket().atJunc && tempPtr->val.checkCollision(&junction)) {
				//Car has just entered junction
				cout << "Car with ID: " << tempPtr->val.getID() << " has entered the junction." << endl;
			}
			else if (tempPtr->val.getPacket().atJunc && !tempPtr->val.checkCollision(&junction)) {
				//Car has just left junction
				tempPtr->val.setLaneID(tempPtr->val.getPacket().intendedLaneID);
				for (listPtr tempPtr2 = mainList; tempPtr2 != NULL; tempPtr2 = tempPtr2->next) {
					//Now make sure any slowed down cars return normal speed
					tempPtr2->val.setCommand(Car::CommandType::Ready);
				}
				cout << "Car with ID: " << tempPtr->val.getID() << " has left the junction." << endl;
			}
		}


	}

	static string cmdString(Car::CommandType cmd) {
		switch (cmd) {
		case 0: return "STOP"; break;
		case 1: return "SLOW"; break;
		case 2: return "READY"; break;
		}
	}

private:
	struct TimeID {
		Lane::LaneType laneID;
		float time;
	};

	bool isStarted;
	int carIndex; //must only be incremented at every creation of a new car.

	//recursive drawing, overloaded function
	void drawList(sf::RenderWindow& windowHandle, listPtr l) {
		if (l != NULL) {
			if (isStarted) { l->val.update(); }
			windowHandle.draw(l->val);
			drawList(windowHandle, l->next);
		}
	}

	static bool compTimeID(TimeID i, TimeID j) { return (i.time < j.time); }


	Car* getCarByID(int ID) {
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (tempPtr->val.getID() == ID) {
				return &tempPtr->val;
			}
		}
		return NULL;
	}
};