#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class Entity : public sf::CircleShape {
public:
	Entity() {
		//if its a default construct do nothing so nothing shows up, neccessary for Car object
	}
	Entity(sf::Color color, float radius, size_t sides = 30Ui64, sf::Vector2f position = sf::Vector2f(0, 0), bool centeredOrigin = true) {
		this->setFillColor(color);
		this->setRadius(radius);
		this->setPointCount(sides);
		if (centeredOrigin) { this->setOrigin(this->getRadius(), this->getRadius()); } //adjust origin before setting position
		this->setPosition(position);
	}
	~Entity() {
		//delete data to pointers;
		//delete this; needed ??
	}
	virtual void update() {
		//get active/latest command here i.e. pause stop etc.
		this->move(this->velocity);
	}
	virtual void setVelocity(sf::Vector2f velocity) {
		this->initVelocity = velocity; //for restart purposes
		this->velocity = velocity;
	}
	virtual bool checkCollision(sf::Shape* entity) {
		//needs to be as general as possible hence sf::Shape this ensures that rectangle shapes can also be tested
		return this->getGlobalBounds().intersects(entity->getGlobalBounds());
	}
protected:
	sf::Vector2f initVelocity;
	sf::Vector2f velocity;
	sf::Vector2f position;
private:
	//sf::Color* colour;
	//sf::CircleShape* object; //The actual object that this entity will create

};