#pragma once

#include <string>
#include <SFML/Graphics.hpp>

struct MazeNode;
class Maze;

#define COST_MOVE 1;

struct NodeLinks
{
	bool north;
	bool south;
	bool east;
	bool west;
};

class Mouse
{
	friend class Maze;
protected:
	std::string _name;
	sf::Color _color;
	void (*_logic)(Mouse*);  // Logic functor.

	int _resources;
	bool spendResources(unsigned amt);

public:
	MazeNode* _loc;

	NodeLinks getLinksAtCurrentNode();
	bool moveDirection(int direction);
};