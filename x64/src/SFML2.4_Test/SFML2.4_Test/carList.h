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

	void addItem(Car& el) {
		el.setID(carIndex++); //This must be called first cause object is passed by val NOT REF on LinkedList::addItem
		LinkedList::addItem(el); //call default command, you dont need virtual for some reason??? (i dont really understand them well)
		//Set the car index to current value and increment it for next addition.
							  //need plus to convert primitive datatype to numeric value, colour.r/g/b is unit8 which is internally a ASCII char
		cout << "Created car with index: " << el.getID() << " and colour : (" << +el.getColor().r << ", " << +el.getColor().g << ", " << +el.getColor().b << ")" << endl;
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

	void processCommands() {
		vector<Car::CommandType> cmds; //vector of cmds
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			cmds.push_back(tempPtr->val.getCommand());
		}
		int cmdScore = 1;
		for (int i = 0; i < cmds.size(); i++) {
			cout << "The car with ID " << i << " has command: " << cmds[i] << endl;
			cmdScore *= cmds[i];
		}
		int maxScore = pow((int)Car::CommandType::Ready, cmds.size());
		double cmdProb = cmdScore / maxScore;
		cout << "The probablity of stopping is: " << (1-cmdProb) << endl;
		if (cmdProb == 0) {
			cout << "DECISION: STOP!!" << endl;
			for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
				tempPtr->val.recieveCommand(Car::CommandType::Stop);//send stop to every car
			}
		}
		
	}

	bool setCommand(int ID, Car::CommandType cmd) {
		for (listPtr tempPtr = mainList; tempPtr != NULL; tempPtr = tempPtr->next) {
			if (tempPtr->val.getID() == ID) {
				tempPtr->val.setCommand(cmd);
				return true;
			}
		}
		return false;
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