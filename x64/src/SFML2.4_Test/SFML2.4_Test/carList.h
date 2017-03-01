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

	void addItem(sf::Color color = sf::Color::Red, sf::Vector2f position = sf::Vector2f(0, 0), sf::Vector2f velocity = sf::Vector2f(0, 0)) {
		Car* tempCar = new Car(color); //custom car
		tempCar->setPosition(position);
		tempCar->setVelocity(velocity);
		tempCar->setID(carIndex++); //This must be called first cause object is passed by val NOT REF on LinkedList::addItem
		LinkedList::addItem(*tempCar); //call default command, you dont need virtual for some reason??? (i dont really understand them well)
		cout << "Created car with index: " << tempCar->getID() << " and colour : (" << +tempCar->getColor().r << ", " << +tempCar->getColor().g << ", " << +tempCar->getColor().b << ")" << endl;
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
		vector<Car::DataPacket> packets; //A vector containing the DataPacket from every car (this gives current status)

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

		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			packets.push_back(tempPtr->val.getPacket);
		}
		//TODO: Do Stuff here @sdpenquin @mohika
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
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (tempPtr->val.getGlobalBounds().left > windowHandle.getSize().x) {
				//reset position
				tempPtr->val.setPosition(0 - CAR_SIZE, tempPtr->val.getPosition().y);
			}
			else if (tempPtr->val.getGlobalBounds().top > windowHandle.getSize().y) {
				//reset position (else, since no diagonal movement)
				tempPtr->val.setPosition(tempPtr->val.getPosition().x, 0 - CAR_SIZE);
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