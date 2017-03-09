#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

//int operator + (const Lane::LaneType & Op1, const Lane::LaneType & Op2);

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

	enum TurningDirection
	{
		Left = 3,
		Forward = 2,
		Right = 1,
		UndefinedDirection = -1
	};

private:
	LaneType laneID;
};

Lane::TurningDirection operator + (const Lane::LaneType & Op1, const Lane::LaneType & Op2) //The spirit of the function - returns turning direction by adding the lanes
{
	int x, y;

	if (Op1 == Lane::LaneType::LaneEI)
	{
		x = 10;
	}
	else if (Op1 == Lane::LaneType::LaneSI)
	{
		x = 20;
	}
	else if (Op1 == Lane::LaneType::LaneWI)
	{
		x = 30;
	}
	else if (Op1 == Lane::LaneType::LaneNI)
	{
		x = 40;
	}
	else
	{
		return Lane::TurningDirection::UndefinedDirection;
	}

	if (Op2 == Lane::LaneType::LaneEO)
	{
		y = 1;
	}
	else if (Op2 == Lane::LaneType::LaneSO)
	{
		y = 2;
	}
	else if (Op2 == Lane::LaneType::LaneWO)
	{
		y = 3;
	}
	else if (Op2 == Lane::LaneType::LaneNO)
	{
		y = 4;
	}
	else
	{
		return Lane::TurningDirection::UndefinedDirection;
	}

	int z = x + y;

	switch (z)
	{
	case(14):
		return Lane::TurningDirection::Right;
		break;
	case(13):
		return Lane::TurningDirection::Forward;
		break;
	case(12):
		return Lane::TurningDirection::Left;
		break;
	case(21):
		return Lane::TurningDirection::Right;
		break;
	case(24):
		return Lane::TurningDirection::Forward;
		break;
	case(23):
		return Lane::TurningDirection::Left;
		break;
	case(32):
		return Lane::TurningDirection::Right;
		break;
	case(31):
		return Lane::TurningDirection::Forward;
		break;
	case(34):
		return Lane::TurningDirection::Left;
		break;
	case(43):
		return Lane::TurningDirection::Right;
		break;
	case(42):
		return Lane::TurningDirection::Forward;
		break;
	case(41):
		return Lane::TurningDirection::Left;
		break;
	default:
		return Lane::TurningDirection::UndefinedDirection;
	}
}
