#include <SFML/Graphics.hpp>
#include <iostream>
#include "entity.h"
#include "car.h"
#include "carList.h"
#include "constants.h"

using namespace std;
using namespace sf;

int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "ITCS Simulation v0.01");
	CarList carList; //instatiate a list of type Car.
	bool isStarted = false;

#pragma region Tracks
	//Entity trackVert(sf::Color::White, 50.f, 4Ui64, Vector2f(window.getSize().x / 2, 0 + CAR_SIZE));
	sf::RectangleShape trackVert(Vector2f(TRACK_WIDTH, window.getSize().y));
	trackVert.setPosition(Vector2f((window.getSize().x - trackVert.getSize().x) / 2,0));

	sf::RectangleShape trackHorz(Vector2f(window.getSize().x, TRACK_WIDTH));
	trackHorz.setPosition(Vector2f(0,(window.getSize().y - trackHorz.getSize().y) / 2));

#pragma endregion


#pragma region InitCars

	//This stuff can be moved to CarList class.
	//on every creation a unique id must be assign by carlist, this is done by carList.addItem
	//carlist will get size of current list, then add one.? or start from 0 as id
	//if id =-1 it tells that id has not yet been set
	//can use id to stop a particular car or change command of particular car.

	Car* tempCar = new Car(); //Default constructor, creates a centered origin 20.f(default) red circle at top left corner
	tempCar->setPosition(window.getSize().x / 2, 0 + CAR_SIZE);
	tempCar->setVelocity(Vector2f(0, 0.4));
	carList.addItem(*tempCar);
	delete tempCar;

	tempCar = new Car(sf::Color::Blue, CAR_SIZE, 30Ui64, Vector2f(0 + CAR_SIZE, window.getSize().y / 2)); //custom car
	tempCar->setVelocity(Vector2f(1, 0));
	carList.addItem(*tempCar);
	delete tempCar;

	tempCar = new Car(sf::Color::Green, CAR_SIZE, 30Ui64, Vector2f(window.getSize().x / 2, window.getSize().y / 2)); //custom car
	tempCar->setVelocity(Vector2f(0.5, 0));
	carList.addItem(*tempCar);
	delete tempCar;
#pragma endregion

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
						cout << (!carList.getAnimStat() ? "Starting" : "Stopping") << " simulation..." << endl;
						carList.setAnimStat(!carList.getAnimStat());
						
						break;
					case 'p':
						cout << "Processing commands..." << endl;
						carList.processCommands();
						break;
					case '0':
						cout << "Setting STOP command to ID 0..." << endl;
						cout << "Command was " << (carList.setCommand(0, Car::CommandType::Stop) ? "set." : "not set.") << endl;
						break;
						
					}
				}

			}
		}

#pragma region Draw
		window.clear();
		window.draw(trackVert);
		window.draw(trackHorz);
		carList.drawList(window);
		window.display();
#pragma endregion

		sleep(seconds(PERIOD_S)); //XXfps, this is only approx
	}

	return 0;
}