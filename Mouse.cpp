#include "Mouse.h"
#include "Maze.h"

bool Mouse::spendResources(unsigned amt)
{
	if(_resources - amt < 0)
		return false;
	else
		_resources -= amt;

	return true;
}

NodeLinks Mouse::getLinksAtCurrentNode()
{
	NodeLinks links = {false, false, false, false};

	if(_loc->link[NORTH])
		links.north = true;
	if(_loc->link[SOUTH])
		links.south = true;
	if(_loc->link[EAST])
		links.east = true;
	if(_loc->link[WEST])
		links.west = true;

	return links;
}

bool Mouse::moveDirection(int direction)
{
	const static int cost = 1;
	if(spendResources(cost) == false)
		return false;

	if(direction < 0 || direction > 3)
		return false;

	if(_loc->link[direction] != 0){
		_loc = _loc->link[direction];
		return true;
	}
}