#include <time.h>
#include <SFML/Graphics.hpp>

#include <iostream>
#include <stack>
#include <map>


#include "Maze.h"
#include "Mice.h"

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 700

int main(int argc, char* argv[])
{
	srand(time(0));
	sf::RenderWindow* app = new sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32), "Maze Generator / Mouse AI");
	sf::Event e;

	Maze maze(WINDOW_WIDTH/20, (WINDOW_HEIGHT - 200)/20);

	maze.registerMouse(random_mouse, "Random Mouse", sf::Color::Blue);
	maze.registerMouse(trace_mouse, "Tracer Mouse", sf::Color::Red);
	maze.registerMouse(clockwise_mouse, "Clockwise Mouse", sf::Color::Green);
	maze.registerMouse(hand_along_wall_mouse, "Hand-on-wall Mouse", sf::Color(120,120,120));

	while(app->isOpen())
	{
		while(app->pollEvent(e))
		{
			if(e.type == sf::Event::Closed)
			{
				app->close();
			}

			maze.handleEvents(e);
		}

		maze.updateMice();

		app->clear(sf::Color::White);
		maze.drawMaze(app);
		app->display();
	}

	return 0;
}

