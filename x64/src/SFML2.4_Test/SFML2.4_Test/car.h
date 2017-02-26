#pragma once

#include "entity.h"
#include "constants.h"
#include <vector>

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
		currentState = CommandType::Ready;
		intention = IntentionType::FollowingRoad;
		uniqueID = -1;

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

	Color getColor() { return this->getFillColor(); }

	void setID(int ID) { uniqueID = ID; }
	int getID() { return uniqueID; }

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
		if (cmdProb == 0 && this->currentState != CommandType::Stop) {
			std::cout << "ID: " << this->uniqueID << "\tDECISION: STOP!!" << "\t(Probability: " << cmdProb << ")" << std::endl;
			this->stop();
		}
		else if (cmdProb == 1 && this->currentState != CommandType::Ready) {
			std::cout << "ID: " << this->uniqueID << "\tDECISION: Continue" << "\t(Probability: " << cmdProb << ")" << std::endl;
			this->restart(); //if slow or stopped travel at random speed.
		}
		else if (cmdProb > 0 && cmdProb < 1 && this->currentState != CommandType::Slow) {
			//For ready state 0 < cmdProb < 1
			//but this can be adjusted to allow for more iterim states
			std::cout << "ID: " << this->uniqueID << "\tDECISION: SLOW" << "\t(Probability: " << cmdProb << ")" << std::endl;
			this->slow(); //if slow or stopped travel at random speed.
		}
		else {
			//continue in the current state
		}
	}


private:
	CommandType command;
	CommandType currentState;
	int uniqueID; //-1 by default? some unique number always?
	IntentionType intention;
	void stop() {
		this->velocity = Vector2f(0, 0);
		this->currentState = CommandType::Stop;
	}
	void slow() {
		if (this->currentState == CommandType::Stop) {
			this->velocity = SLOW_MULT * this->initVelocity;
		}
		else if (this->currentState == CommandType::Ready) {
			this->velocity = SLOW_MULT * this->velocity;
		}
		this->currentState = CommandType::Slow;
	}
	void restart() {
		this->velocity = this->initVelocity;
		this->currentState = CommandType::Ready;
	}
};