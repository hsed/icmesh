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
		//Gather all packets -> TX
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			allPackets.push_back(tempPtr->val.getPacket()); //needed later
		}

		//push all gathered packets to every car, same as mesh network -> RX
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			tempPtr->val.recievePackets(allPackets);
		}
		

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

	Car* getCarByID(int ID) {
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (tempPtr->val.getID() == ID) {
				return &tempPtr->val;
			}
		}
		return NULL;
	}

private:

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

	



};