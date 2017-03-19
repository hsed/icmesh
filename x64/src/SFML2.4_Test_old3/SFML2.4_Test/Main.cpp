#include <SFML/Graphics.hpp>
#include <iostream>
#include "entity.h"
#include "car.h"
#include "carList.h"
#include "constants.h"
#include "lane.h"

using namespace std;
using namespace sf;

void initShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList, Lane& junction);
void drawShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList, Lane& junction);

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), ABOUT); //Main window
	//sf::RenderWindow window2(sf::VideoMode(400, 400), "Information Window"); //Info Window, use it if we need it
	
	Lane junction;				//not really a lane but have similar properties aka an intersection is of rectangular type.
	vector<Lane> laneList;		//a vector with objects of type "Lane".
	CarList carList;			//a class with an underlying linked-list with objects of type "Car", similar to vector but better memory management.
	bool isStarted = false;

	cout << ABOUT << endl  << endl;
	
	initShapes(window, laneList, carList, junction); //define shapes for the window

	while (window.isOpen())
	{
		sf::Event event;
		//sf::Event Event2;
		while (window.pollEvent(event))
		{
			/*window2.pollEvent(Event2);//this is not enough you need to loop through >>> (What does this mean?)
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
						cout << "Unfreezing";
						break;
					}
				}
			}
		}

		if (carList.getAnimStat()) { carList.processCommands(); }//process commands continuously--disable for testing
		carList.checkJunction(junction);
		carList.checkPositions(window);
		drawShapes(window, laneList, carList, junction);

		sleep(seconds(PERIOD_S)); //XXfps, this is only approx
	}

	return 0;
}

void initShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList, Lane& junction)
{
	//Init Tracks
	//Vertical Tracks
	laneList.push_back(Lane(Lane::LaneNO, Vector2f(TRACK_WIDTH, (float)window.getSize().y/2), Vector2f((window.getSize().x/2 - TRACK_WIDTH), 0), Color(150, 200, 200)));
	laneList.push_back(Lane(Lane::LaneNI, Vector2f(TRACK_WIDTH, (float)window.getSize().y / 2), Vector2f((window.getSize().x/2), 0), Color(60,120,120)));
	laneList.push_back(Lane(Lane::LaneSI, Vector2f(TRACK_WIDTH, (float)window.getSize().y / 2), Vector2f((window.getSize().x / 2 - TRACK_WIDTH), (float)window.getSize().y / 2), Color(50, 150, 80)));
	laneList.push_back(Lane(Lane::LaneSO, Vector2f(TRACK_WIDTH, (float)window.getSize().y / 2), Vector2f((window.getSize().x / 2), (float)window.getSize().y / 2), Color(20, 100, 60)));

	//Horizontal Tracks
	laneList.push_back(Lane(Lane::LaneEO, Vector2f((float)window.getSize().x/2, TRACK_WIDTH), Vector2f(0, (window.getSize().y/2 - TRACK_WIDTH)), Color(120, 40, 60)));
	laneList.push_back(Lane(Lane::LaneEI, Vector2f((float)window.getSize().x / 2, TRACK_WIDTH), Vector2f(0, (window.getSize().y / 2)), Color(80, 10, 20)));
	laneList.push_back(Lane(Lane::LaneWI, Vector2f((float)window.getSize().x / 2, TRACK_WIDTH), Vector2f((float)window.getSize().x / 2, (window.getSize().y / 2 - TRACK_WIDTH)), Color(100, 40, 100)));
	laneList.push_back(Lane(Lane::LaneWO, Vector2f((float)window.getSize().x / 2, TRACK_WIDTH), Vector2f((float)window.getSize().x / 2, (window.getSize().y / 2)), Color(60, 20, 60)));
	//trackHorz.setSize();
	//trackHorz.setPosition();


	//Init Cars
	carList.addItem(Lane::LaneNI,Lane::LaneSO,sf::Color::Red, Vector2f(((float)window.getSize().x + TRACK_WIDTH) / 2, 0 + 150), Vector2f(0, 0.4f));
	carList.addItem(Lane::LaneWI, Lane::LaneEO, sf::Color::Blue, Vector2f(20 + CAR_SIZE, ((float)window.getSize().y - TRACK_WIDTH) / 2), Vector2f(1.f, 0));
	carList.addItem(Lane::LaneEI, Lane::LaneWO, sf::Color::Green, Vector2f(((float)window.getSize().x + TRACK_WIDTH) / 2 + 100, ((float)window.getSize().y + TRACK_WIDTH) / 2), Vector2f(-0.5f, 0));

	//Initi Junction // bad programming
	junction = Lane(Lane::Undefined, Vector2f(TRACK_WIDTH*2, TRACK_WIDTH*2), Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), Color::Yellow);
	junction.setOrigin(Vector2f(junction.getSize().x / 2, junction.getSize().y / 2));
}

void drawShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList, Lane& junction) {
	//draw all items on screen;
	window.clear();
	for (int i = 0; i < laneList.size(); i++) {
		window.draw(laneList[i]);
	}
	window.draw(junction);

	//Draw last
	carList.drawList(window);
	window.display();
}