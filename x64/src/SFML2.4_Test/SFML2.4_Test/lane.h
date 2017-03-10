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

	Lane() {
		//default, needed for lane
		this->laneID = Undefined;
	}
	Lane(LaneType ID, Vector2f size, Vector2f position, Color color = sf::Color::White) {
		this->laneID = ID;
		this->setSize(size);
		this->setPosition(position);
		this->setFillColor(color);
	}
	~Lane(){}

	LaneType getLaneID() { return laneID; }
	//LaneID only set when constructing object.

	struct LaneTime {
		LaneType laneID;
		float time;
	};

	static std::string laneString(Lane::LaneType ID) {
		switch (ID) {
		case 0: return "North-Out";
		case 1: return "North-In";
		case 2: return "East-Out";
		case 3: return "East-In";
		case 4: return "South-Out";
		case 5: return "South-In";
		case 6: return "West-Out";
		case 7: return "West-In";
		}
	}

	static bool canCrash(Lane::LaneType ID1Cur, Lane::LaneType ID2Cur, Lane::LaneType ID1Fut, Lane::LaneType ID2Fut) {
		if (ID1Fut == ID2Fut) {
			return true;	//Same intended lane
		}
		else if ((ID1Cur == 7 && ID2Cur == 1) || (ID1Cur == 1 && ID2Cur == 7)) {
			return true; //special case
		}
		else if ((ID1Cur == 3 && ID2Cur == 5) || (ID1Cur == 5 && ID2Cur == 3)) {
			return true; //special case
		}
		else {
			return false; //default case
		}
	}

private:
	LaneType laneID;



};

