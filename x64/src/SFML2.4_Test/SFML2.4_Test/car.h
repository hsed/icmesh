#pragma once

#include "entity.h"

using namespace sf;

class Car : public Entity {

public:
	Car(){
		this->setFillColor(sf::Color::Red);
		this->setRadius(20.f);
		this->setOrigin(this->getRadius(), this->getRadius()); //adjust origin before setting position
		this->setPosition(this->getRadius(), this->getRadius());
		//std::cout << "bullshita" << std::endl;
	}
};