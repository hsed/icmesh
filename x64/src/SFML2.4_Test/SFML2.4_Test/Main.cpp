#include <SFML/Graphics.hpp>
#include <iostream>
#include "entity.h"
#include "car.h"
#include "carList.h"
#include "constants.h"
#include "lane.h"

using namespace std;
using namespace sf;

void initShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList);
void drawShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList);

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), ABOUT);
	sf::RectangleShape trackVert;
	sf::RectangleShape trackHorz;

	vector<Lane> laneList;

	CarList carList; //a linked list with objects for each node of type Car.
	bool isStarted = false;

	cout << ABOUT << endl  << endl;
	
	initShapes(window, laneList, carList); //define shapes for the window

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128) {
					char c = static_cast<char>(event.text.unicode);
					//cout << "ASCII character typed: " << c << endl;
					switch (c) {
					case 's':
						cout << (!carList.getAnimStat() ? "\nStarting" : "\nStopping") << " simulation..." << endl;
						carList.setAnimStat(!carList.getAnimStat());

						break;
					case 'p':
						cout << "\nThis feature is disabled as processing is continuous..." << endl;
						//carList.processCommands();
						break;
					case '0':
						cout << "\nSTOP command for ID 0 was " << (carList.setCommand(0, Car::CommandType::Stop) ? "set." : "not set.") << endl;
						break;
					case '1':
						cout << "\nSLOW command for ID 0 was " << (carList.setCommand(0, Car::CommandType::Slow) ? "set." : "not set.") << endl;
						break;
					case '2':
						cout << "\nREADY command for ID 0 was " << (carList.setCommand(0, Car::CommandType::Ready) ? "set." : "not set.") << endl;
						break;
					}
				}

			}
		}

		carList.processCommands(); //process commands continuously
		carList.checkPositions(window);
		drawShapes(window, laneList, carList);

		sleep(seconds(PERIOD_S)); //XXfps, this is only approx
	}

	return 0;
}

void initShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList)
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
	carList.addItem(sf::Color::Red, Vector2f(((float)window.getSize().x + TRACK_WIDTH) / 2, 0 + CAR_SIZE), Vector2f(0, 0.4f));
	carList.addItem(sf::Color::Blue, Vector2f(0 + CAR_SIZE, ((float)window.getSize().y - TRACK_WIDTH) / 2), Vector2f(1.f, 0));
	carList.addItem(sf::Color::Green, Vector2f(((float)window.getSize().x + TRACK_WIDTH) / 2, ((float)window.getSize().y + TRACK_WIDTH) / 2), Vector2f(-0.5f, 0));
}

void drawShapes(sf::RenderWindow& window, vector<Lane>& laneList, CarList& carList) {
	//draw all items on screen;
	window.clear();
	for (int i = 0; i < laneList.size(); i++) {
		window.draw(laneList[i]);
	}
	carList.drawList(window);
	window.display();
}