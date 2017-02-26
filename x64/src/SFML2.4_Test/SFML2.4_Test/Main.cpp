#include <SFML/Graphics.hpp>
#include <iostream>
#include "entity.h"
#include "car.h"
#include "carList.h"
#include "constants.h"

using namespace std;
using namespace sf;

void initShapes(sf::RenderWindow& window, sf::RectangleShape& trackVert, sf::RectangleShape& trackHorz, CarList& carList);
void drawShapes(sf::RenderWindow& window, sf::RectangleShape& trackVert, sf::RectangleShape& trackHorz, CarList& carList);

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), ABOUT);
	sf::RectangleShape trackVert;
	sf::RectangleShape trackHorz;
	CarList carList; //a linked list with objects for each node of type Car.
	bool isStarted = false;

	cout << ABOUT << endl  << endl;
	
	initShapes(window, trackVert, trackHorz, carList); //define shapes for the window

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
		drawShapes(window, trackVert, trackHorz, carList);

		sleep(seconds(PERIOD_S)); //XXfps, this is only approx
	}

	return 0;
}

void initShapes(sf::RenderWindow& window, sf::RectangleShape& trackVert, sf::RectangleShape& trackHorz, CarList& carList)
{
	//Init Tracks
	trackVert.setSize(Vector2f(TRACK_WIDTH, (float)window.getSize().y));
	trackVert.setPosition(Vector2f((window.getSize().x - trackVert.getSize().x) / 2, 0));

	trackHorz.setSize(Vector2f((float)window.getSize().x, TRACK_WIDTH));
	trackHorz.setPosition(Vector2f(0, (window.getSize().y - trackHorz.getSize().y) / 2));


	//Init Cars
	carList.addItem(sf::Color::Red, Vector2f((float)window.getSize().x / 2, 0 + CAR_SIZE), Vector2f(0, 0.4f));
	carList.addItem(sf::Color::Blue, Vector2f(0 + CAR_SIZE, (float)window.getSize().y / 2), Vector2f(1.f, 0));
	carList.addItem(sf::Color::Green, Vector2f((float)window.getSize().x / 2, (float)window.getSize().y / 2), Vector2f(0.5f, 0));
}

void drawShapes(sf::RenderWindow& window, sf::RectangleShape& trackVert, sf::RectangleShape& trackHorz, CarList& carList) {
	//draw all items on screen;
	window.clear();
	window.draw(trackVert);
	window.draw(trackHorz);
	carList.drawList(window);
	window.display();
}