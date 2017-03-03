#pragma once

#include "entity.h"
#include "constants.h"
#include "lane.h"
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
		currentCommand = CommandType::Ready;

		currentState = {-1, Lane::Undefined, Lane::Undefined, -1, -1}; //need to implement speed and position from intersection

	}


	struct DataPacket {
		int carID;
		Lane::LaneType laneID;
		Lane::LaneType intendedLaneID;
		float speed; //direction is implied by other properties so this is Speed in X or Y
		float relDist; //relative distance from intersection (assume google maps)
	};

	void update() {
		Entity::update(); //important
		
	}
	void setVelocity(sf::Vector2f velocity) {
		Entity::setVelocity(velocity); //important
		this->currentState.speed = ((velocity.x == 0) ? velocity.y: velocity.x); //spped has no direction so need to set x or y or either if both zero
	}

#pragma region Commands
	enum CommandType {
		Stop,	//Hazard
		Slow,	//Caution
		Ready
	};
	CommandType getCommand() { return command; }
	CommandType setCommand(CommandType command) { return this->command = command; }
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
	}
#pragma endregion

	Color getColor() { return this->getFillColor(); }

	void setID(int ID) { currentState.carID = ID; }
	int getID() { return currentState.carID; }

	DataPacket getPacket() { return this->currentState; }
	

private:
	DataPacket currentState;

	CommandType command;		//old
	CommandType currentCommand; //old

	
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