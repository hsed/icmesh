#include <SFML/Graphics.hpp>
#include <iostream>
#include "entity.h"
#include "car.h"

using namespace std;
using namespace sf;

int main()
{
	sf::RenderWindow window(sf::VideoMode(640, 480), "SFML works!");
	sf::CircleShape shape(20.f);
	bool isStarted = false;

	Entity* ball1 = new Entity(sf::Color::Blue, 20.f, 30Ui64, Vector2f(0 + 20.f, window.getSize().y / 2), true);
	Entity* ball2 = new Entity(sf::Color::Green, 20.f, 30Ui64, Vector2f(window.getSize().x / 2, window.getSize().y / 2), true);
	ball1->setVelocity(Vector2f(1, 0));
	ball2->setVelocity(Vector2f(0.5, 0));

	Car* car = new Car();
	car->setVelocity(Vector2f(0.6, 0));

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
						cout << "Starting simulation..." << endl;
						isStarted = true;
						break;
					}
				}

			}
		}
		
		if (ball1->checkCollision(ball2)) {
			cout << "Collision!!!" << endl;
		}

		if (isStarted) {
			//shape.move(0.5, 0); //Move shape by one unit diagonally
			ball1->update();
			ball2->update();
			car->update();
		}

		window.clear();
		window.draw(*ball1);
		window.draw(*ball2);
		window.draw(*car);


		window.display();
		sleep(seconds(0.01f)); //XXfps, this is only approx
	}

	return 0;
}