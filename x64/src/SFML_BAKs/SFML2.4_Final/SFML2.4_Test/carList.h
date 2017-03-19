#pragma once
#include "car.h"
#include <vector>

using namespace std;
using namespace sf;

class CarList : public vector<Car*> {
	//This class inherits LinkedList... no sorry it inherits vector!!
	//To access a car's member_func: this->at(i)->member_func(); //where i is the iterator, this[i] doesn't work :(
public:
	CarList() {
		initParams();
	}
	~CarList() {

	}
	CarList(int numCars) {
		initParams();
		for (int i = 0; i < numCars; i++) {
			//create new randomcar
			addItem();
		}
	}
	void addItem() {
		//To generate a random car, pass no arguments === default constructor
		//Car* tempCar = new Car(Car::getRandColor()); //custom car
		
		Lane::LaneType laneID = Lane::getRandLane(); //no previous lane specified so this is the starting lane
		Lane::LaneType intendedLaneID = Lane::getRandLane(laneID); //previous lane to ensure no u-turn
		Color color = Car::getRandColor();
		Vector2f pos = Lane::getRandInPos(laneID);
		
		/*Position initial collision detection*/
		bool isCollide = true; //assume collision initially
		Car* tempCar = new Car;
		while (isCollide) {
			isCollide = false; //assume false once inside the while loop.
			tempCar->setPosition(pos);
			for (int i = 0; i < this->size() && !isCollide; i++) {
				if (this->at(i)->checkCollision(tempCar)) {
					//it collides so get another random position.
					laneID = Lane::getRandLane(); //get another lane as original lane maybe full.
					intendedLaneID = Lane::getRandLane(laneID);
					pos = Lane::getRandInPos(laneID);
					isCollide = true; //this will break for loop but not while loop.
				}
			}
		}
		delete tempCar; //prevent memory leak.

		Vector2f vel = laneVelocities[laneID]; //assume already initiliased in initPAram;
		this->addItem(laneID, intendedLaneID, color, pos, vel);
	}
	void addItem(Lane::LaneType laneID, Lane::LaneType intendedLaneID, sf::Color color, sf::Vector2f position, sf::Vector2f velocity) {
		
		Car* tempCar = new Car(color); //custom car
		tempCar->setPosition(position);
		tempCar->setVelocity(velocity);
		tempCar->setID(carIndex++); //This must be called first cause object is passed by val NOT REF on LinkedList::addItem
		tempCar->setLaneID(laneID);
		tempCar->setIntLaneID(intendedLaneID);
		this->push_back(tempCar); //call default command, you dont need virtual for some reason??? (i dont really understand them well)
		cout << "Created car with index: " << tempCar->getID() << ", colour: (" << +tempCar->getColor().r << ", " << +tempCar->getColor().g << ", " << +tempCar->getColor().b << ")";
		cout << ", laneID: " << tempCar->getPacket().laneID << " and intendedLaneID: " << tempCar->getPacket().intendedLaneID << endl;
		//delete tempCar; //free up memory!
	}

	bool getAnimStat() {
		return isStarted;
	}
	void setAnimStat(bool start) {
		isStarted = start;
	}


	void drawList(sf::RenderWindow& windowHandle) {
		if (this->size() > 0) {
			for (int i = 0; i < this->size(); i++) {
				if (isStarted) { this->at(i)->update(); }
				windowHandle.draw(*this->at(i));
			}

		}
	}

	//MAIN PROCESSING MODULE
	void processCommands() {
		if (DEBUG) cout << endl << endl;
		//vector<Car::DataPacket> packets = new vector<Car::DataPacket>[LANES]; 
		vector<vector<Car::DataPacket>> packetsPerLane;
		vector<Car::DataPacket> tmpLanePackets, allPackets;

		//An array of vector containing the DataPacket from every car (this gives current status)
		//Each array element is for one lane only.

		//leave this for now
#pragma region Commands
		vector<Car::CommandType> cmds; //vector list of commands
		//gather the commands from all cars in a vector, same as mesh network -> TX
		for (int i = 0; i < this->size(); i++) {
			cmds.push_back(this->at(i)->getCommand());
		}

		//push all gathered commands to evry car, same as mesh network -> RX
		for (int i = 0; i < this->size(); i++) {
			this->at(i)->recieveCommands(cmds);
		}
#pragma endregion

		//TODO: Do Stuff here @sdpenquin @mohika
		//Gather all packets -> TX
		for (int i = 0; i < this->size(); i++) {
			allPackets.push_back(this->at(i)->getPacket());
		}

		//push all gathered packets to every car, same as mesh network -> RX
		for (int i = 0; i < this->size(); i++) {
			this->at(i)->recievePackets(allPackets);
		}
		

	}

	bool setCommand(int ID, Car::CommandType cmd) {
		//Set a command for a particular car given by ID
		for (int i = 0; i < this->size(); i++){
			if (this->at(i)->getID() == ID) {
				this->at(i)->setCommand(cmd);
				return true;
			}
		}
		return false;
	}

	void checkPositions(sf::RenderWindow& windowHandle) {
		//imp! bounds are top-left corner origin but cars themselves are centered origin
		for (int i = 0; i < this->size(); i++) {
			Car* tempPtr = this->at(i);
			if (tempPtr->getGlobalBounds().left > windowHandle.getSize().x && !carNearEdge(Lane::LaneWI)) {
				//reset position to WI

				tempPtr->setPosition(0 - CAR_RADIUS, tempPtr->getPosition().y);
				tempPtr->setLaneID(Lane::LaneWI);
			}
			else if (tempPtr->getGlobalBounds().left < 0 - CAR_RADIUS * 2) {
				//reset position to EI
				tempPtr->setPosition(windowHandle.getSize().x + CAR_RADIUS, tempPtr->getPosition().y);
				tempPtr->setLaneID(Lane::LaneEI);
			}
			else if (tempPtr->getGlobalBounds().top > windowHandle.getSize().y) {
				//reset position (else, since no diagonal movement) to NI
				tempPtr->setPosition(tempPtr->getPosition().x, 0 - CAR_RADIUS);
				tempPtr->setLaneID(Lane::LaneNI);
			}
			else if (tempPtr->getGlobalBounds().top < 0 - CAR_RADIUS * 2) {
				//reset position to SI
				tempPtr->setPosition(tempPtr->getPosition().x, windowHandle.getSize().y + CAR_RADIUS);
				tempPtr->setLaneID(Lane::LaneSI);
			}
		}

	}

	void checkJunction(Lane& junction) {
		for (int i = 0; i < this->size(); i++) {
			Car* tempPtr = this->at(i);
			if (!tempPtr->getPacket().atJunc && tempPtr->checkCollision(&junction)) {
				//Car has just entered junction
				cout << "Car with ID: " << tempPtr->getID() << " has entered the junction." << endl;
			}
			else if (tempPtr->getPacket().atJunc && !tempPtr->checkCollision(&junction)) {
				//Car has just left junction
				tempPtr->setLaneID(tempPtr->getPacket().intendedLaneID);
				for (int i = 0; i < this->size(); i++) {
					//Now make sure any slowed down cars return normal speed
					this->at(i)->setCommand(Car::CommandType::Ready);
				}
				cout << "Car with ID: " << tempPtr->getID() << " has left the junction." << endl;
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
		for (int i = 0; i < this->size(); i++) {
			if (this->at(i)->getID() == ID) {
				return this->at(i);
			}
		}
		return NULL;
	}

private:

	bool isStarted;
	int carIndex; //must only be incremented at every creation of a new car.
	vector<Vector2f> laneVelocities; //just so that i don't need to search it in cars;

	void initParams() {
		//used by default constructor or any constructor for that matter;
		isStarted = false;
		carIndex = 0;
		for (int i = 0; i < LANES; i++) {
			laneVelocities.push_back(Vector2f(Lane::getRandVel((Lane::LaneType)(i)))); //i gives lane_id and Lane then returns a random vel VALID for that lane e.g. +- etc.
			//laneVelocities.push_back(Vector2f(0,0)); //CHANGE THIS!!!
			cout << "vel: (" << laneVelocities[i].x << ", " << laneVelocities[i].y << ")" << endl;
		}
	}
	
	bool carNearEdge(Lane::LaneType l) {
		float diff;
		float maxRelDist = -1;
		for (int i = 0; i < this->size(); i++) {
			Car* tempPtr = this->at(i);
			if (tempPtr->getPacket().laneID == l) {
				if (tempPtr->getPacket().relDist > maxRelDist) maxRelDist = tempPtr->getPacket().relDist; //get largest dist from intersection for all cars in lane

			}
			
		}
		//change code here
		diff = Lane::getMaxLaneLength(l) - maxRelDist;
		if (diff >= CAR_RADIUS*2) { return false; } //if there is a car in the first two car spaces in lane then there is a car near edge
		else { return true; }
	}



};