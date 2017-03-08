#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Lane : public sf::RectangleShape {
public:
	enum LaneType {
		Undefined = -1, //For initialisation
		LaneNO = 0, //North Outward
		LaneNI = 1, //North Inward
		LaneEO = 2, //East Outward
		LaneEI = 3,
		LaneSO = 4,
		LaneSI = 5,
		LaneWO = 6,
		LaneWI = 7
	};

	//Lane() {}//default
	Lane(LaneType ID, Vector2f size, Vector2f position, Color color = sf::Color::White) {
		this->laneID = ID;
		this->setSize(size);
		this->setPosition(position);
		this->setFillColor(color);
	}
	~Lane(){}

	LaneType getLaneID() { return laneID; }
	//LaneID only set when constructing object.

private:
	LaneType laneID;



};

