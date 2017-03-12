#include "car.h"

Car::Car(sf::Color color, float radius, size_t sides, sf::Vector2f position, bool centeredOrigin) {
	//Default Constructor as all arguments are optional
	this->setFillColor(color);
	this->setRadius(radius);
	this->setPointCount(sides);
	if (centeredOrigin) { this->setOrigin(this->getRadius(), this->getRadius()); } //adjust origin before setting position
	this->setPosition(position);

	command = CommandType::Ready;
	currentCommand = CommandType::Ready;

	currentState = { -1, Lane::Undefined, Lane::Undefined, -1, calculateDistFromJunc(), -1, false }; //need to implement speed and position from intersection

}
