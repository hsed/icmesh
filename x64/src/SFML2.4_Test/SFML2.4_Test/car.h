#pragma once

#include "entity.h"

using namespace sf;

class Car : public Entity {

public:
	Car(sf::Color colour = sf::Color::Red, float radius = 20.f, size_t sides = 30Ui64, sf::Vector2f position = sf::Vector2f(0, 0), bool centeredOrigin = true) {
		//Default Constructor as all arguments are optional
		this->setFillColor(colour);
		this->setRadius(radius);
		this->setPointCount(sides);
		if (centeredOrigin) { this->setOrigin(this->getRadius(), this->getRadius()); } //adjust origin before setting position
		this->setPosition(position);
		
		command = CommandType::Ready;
		intention = IntentionType::FollowingRoad;
		uniqueID = -1;

		//std::cout << "bullshitinherited" << std::endl;
	}
	

	enum CommandType {
		Stop,	//Hazard
		Slow,	//Caution
		Ready
	};

	enum IntentionType {
		TurningLeft,
		TurningRight,
		FollowingRoad,
		SlowingDown,
		Stopping,
	};

	CommandType getCommand() { return command; }
	CommandType setCommand(CommandType command) { return this->command = command; }

	void recieveCommand(CommandType command) {
		//should be private or protected in future??
		if (command == CommandType::Stop) {
			stop();
		}
	}

	Color getColor() {	return this->getFillColor(); }
	
	void setID(int ID) { uniqueID = ID; }
	int getID(){ return uniqueID; }

private:
	CommandType command;
	int uniqueID; //-1 by default? some unique number always?
	IntentionType intention;
	void stop() {
		this->velocity = Vector2f(0, 0);
	}
	void restart() {

	}
};