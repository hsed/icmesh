#include <SFML/Graphics.hpp>
#include <iostream>
#include "entity.h"
#include "car.h"
#include "carList.h"
#include "constants.h"
#include "lane.h"

using namespace std;
using namespace sf;

void initShapes(sf::RenderWindow& window, vector<Lane>& laneList, vector<Car>& carVec, Lane& junction);
void drawShapes(sf::RenderWindow& window, vector<Lane>& laneList, vector<Car>& carVec, Lane& junction);

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), ABOUT,sf::Style::Close|sf::Style::Titlebar); //Main window
	//Window window2(sf::VideoMode(400, 400), "Information Window",sf::Style::Resize,0); //Info Window, use it if we need it
	
	Lane junction;				//not really a lane but have similar properties aka an intersection is of rectangular type.
	vector<Lane> laneList;		//a vector with objects of type "Lane".
	CarList carList;			//a class with an underlying linked-list with objects of type "Car", similar to vector but better memory management.
	vector<Car> carVec;
	
	
	bool isStarted = false;

	cout << ABOUT << endl  << endl;
	
	initShapes(window, laneList, carVec, junction); //define shapes for the window

	while (window.isOpen())
	{
		sf::Event event;
		//sf::Event Event2;
		while (window.pollEvent(event))
		{
			/*window2.pollEvent(Event2);//this is not enough you need to loop through
			if (Event2.type == sf::Event::Closed) //WHY DOESN'T THIS WORK!?
			{
				window2.close();
			}*/
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128) {
					char c = static_cast<char>(event.text.unicode);
					//cout << "ASCII character typed: " << c << endl;
					switch (c) {
					case 's': //START
						cout << (!carList.getAnimStat() ? "\nStarting" : "\nStopping") << " simulation..." << endl;
						carList.setAnimStat(!carList.getAnimStat());
						break;
					case 'p': //PROCESS CMDS Manually
						carList.processCommands();
						break;
					case 'q': //QUIT
						window.close();
						break;
/*					case '0':
						cout << "\nSTOP command for ID 0 was " << (carList.setCommand(0, Car::CommandType::Stop) ? "set." : "not set.") << endl;
						break;
					case '1':
						cout << "\nSLOW command for ID 0 was " << (carList.setCommand(0, Car::CommandType::Slow) ? "set." : "not set.") << endl;
						break;
					case '2':
						cout << "\nREADY command for ID 0 was " << (carList.setCommand(0, Car::CommandType::Ready) ? "set." : "not set.") << endl;
						break;*/
					case 'f':
						//alternate pausing mechanism
						cout << "FREEZING ... Press 's' to continue.";
						while (static_cast<char>(event.text.unicode) != 's')
						{
							window.pollEvent(event);
							sleep(seconds(0.01));
						}
						cout << "\nUnfreezing\n";
						break;
					}
				}
			}
		}

		//if (carList.getAnimStat()) { carList.processCommands(); }//process commands continuously--disable for testing
		//carList.checkJunction(junction);
		//carList.checkPositions(window);

		for (int i = 0; i < carVec.size(); i++) {
			carVec[i].update();
		}


		drawShapes(window, laneList, carVec, junction);
		//carList.getCarByID(1)->setPosition(laneList[5].GetStartOfLane().x, laneList[5].GetStartOfLane().y);
		sleep(seconds(PERIOD_S)); //XXfps, this is only approx
	}

	return 0;
}

void initShapes(sf::RenderWindow& window, vector<Lane>& laneList, vector<Car>& carVec, Lane& junction)
{
	//Init Tracks
	//Vertical Tracks
	float windowX = window.getSize().x;
	float windowY = window.getSize().y;

	float laneLength = (windowX*(1 - JUNCTIONSIZE)) / 2;
	Vector2f LaneSize = Vector2f(TRACK_WIDTH, laneLength);
	float HalfTrack = TRACK_WIDTH / 2;

	laneList.push_back(Lane(Lane::LaneNO, LaneSize, Vector2f((window.getSize().x/2 - HalfTrack), 0),	0,Color(150, 200, 200)));
	laneList.push_back(Lane(Lane::LaneNI, LaneSize, Vector2f((window.getSize().x/2+ HalfTrack), 0)       		,	0,		Color(60,120,120)));


	laneList.push_back(Lane(Lane::LaneEO, LaneSize, Vector2f((window.getSize().x), (window.getSize().y / 2 - HalfTrack)), 90, Color(120, 40, 60)));
	laneList.push_back(Lane(Lane::LaneEI, LaneSize, Vector2f((window.getSize().x), (window.getSize().y / 2 + HalfTrack)), 90, Color(80, 10, 20)));

	
	
	laneList.push_back(Lane(Lane::LaneSO, LaneSize, Vector2f((windowX / 2 + HalfTrack), windowY), 180, Color(20, 100, 60)));
	laneList.push_back(Lane(Lane::LaneSI, LaneSize, Vector2f((windowX/ 2 -HalfTrack), windowY),180, Color(50, 150, 80)));






	//Horizontal Tracks
	laneList.push_back(Lane(Lane::LaneWO, LaneSize, Vector2f(0, (windowY / 2) + HalfTrack), 270, Color(60, 20, 60)));
	laneList.push_back(Lane(Lane::LaneWI, LaneSize, Vector2f(0, (windowY / 2) -HalfTrack),270, Color(100, 40, 100)));

	//trackHorz.setSize();
	//trackHorz.setPosition();
	for (int i = 0; i < laneList.size(); i++) {
		laneList[i].setOrigin(TRACK_WIDTH / 2, 0);
	}

	for (int i = 0; i < 100; i++) {

		Lane::LaneType Start = (Lane::LaneType)(((rand()%4)*2)-1);
			
			Car C;//custom car

			C.ResetCar(laneList,Lane::LaneType::LaneSI);
			carVec.push_back(C);

	}



	//Initi Junction // bad programming
	//junction = Lane(Lane::Undefined, Vector2f(TRACK_WIDTH*2, TRACK_WIDTH*2), Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), Color::Yellow);
	//junction.setOrigin(Vector2f(junction.getSize().x / 2, junction.getSize().y / 2));
}

void drawShapes(sf::RenderWindow& window, vector<Lane>& laneList, vector<Car>& carVec, Lane& junction) {
	//draw all items on screen;
	window.clear();
	for (int i = 0; i < laneList.size(); i++) {
		window.draw(laneList[i]);
	}
	window.draw(junction);

	//Draw last
	//carList.drawList(window);
	
	for (int i = 0; i < carVec.size(); i++) {
		window.draw(carVec[i]);
	}
	
	window.display();
}