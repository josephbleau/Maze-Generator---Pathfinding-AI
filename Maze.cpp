#include "Maze.h"

#include <stack>
#include <iostream>
#include <sstream>

#include "Mouse.h"

Maze::Maze(unsigned w, unsigned h)
{
	sf::Clock timer;

	_mouse_update_ms = 128;
	_maze_w = w;
	_maze_h = h;
	_draw_size = 20;
	_sprite_updated = false;

	if(!_font.loadFromFile("segoesc.ttf"))
	{
		std::cout << "ERROR LOADING FONT";
	}

	_texture = new sf::RenderTexture();
	_texture->create(_maze_w * _draw_size, _maze_h * _draw_size);

	_maze = std::vector<std::vector<MazeNode*>>();
	for(unsigned i = 0; i < w; i++){
		_maze.push_back(std::vector<MazeNode*>());
		for(unsigned j = 0; j < h; j++)
		{
			_maze[i].push_back(0);
		}
	}

	generate();

	std::cout << "Generation complete (" << timer.getElapsedTime().asMilliseconds() << "ms.)" << std::endl;
}

Maze::~Maze()
{
	for(unsigned i = 0; i < _maze_w; i++)
	{
		for(unsigned j = 0; j < _maze_h; j++)
		{
			if(_maze[i][j])
				delete _maze[i][j];
		}
	}

	if(_texture)
		delete _texture;
}

void Maze::generate()
{
	std::stack<MazeNode*> trace;

	int loc_x = 0;
	int loc_y = 0;

	instantiateMazeNode(loc_x, loc_y);
	trace.push(_maze[loc_x][loc_y]);

	bool attempted[4] = {false, false, false, false};
	while(trace.size() > 0)
	{
		int direction = rand()%4;
		bool moved = false;

		switch(direction)
		{
		case NORTH:
			attempted[NORTH] = isNodeAt(loc_x, loc_y-1);
			if(moved = (attempted[NORTH] == false))
			{
				MazeNode *n = instantiateMazeNode(loc_x, loc_y - 1);
				link(_maze[loc_x][loc_y--], n, NORTH);
			}
			break;
		case EAST:
			attempted[EAST] = isNodeAt(loc_x+1, loc_y);
			if(moved = (attempted[EAST] == false))
			{
				MazeNode *n = instantiateMazeNode(loc_x+1, loc_y);
				link(_maze[loc_x++][loc_y], n, EAST);
			}		
			break;
		case SOUTH:
			attempted[SOUTH] = isNodeAt(loc_x, loc_y+1);
			if(moved = (attempted[SOUTH] == false))
			{
				MazeNode *n = instantiateMazeNode(loc_x, loc_y + 1);
				link(_maze[loc_x][loc_y++], n, SOUTH);
			}
			break;
		case WEST:
			attempted[WEST] = isNodeAt(loc_x-1, loc_y);
			if(moved = (attempted[WEST] == false))
			{
				MazeNode *n = instantiateMazeNode(loc_x-1, loc_y);
				link(_maze[loc_x--][loc_y], n, WEST);
			}
			break;
		}

		if(moved)
		{
			trace.push(_maze[loc_x][loc_y]);
			for(unsigned i = 0; i < 4; i++)
				attempted[i] = false;
			continue;
		}

		if(attempted[NORTH] && attempted[EAST] &&
			attempted[SOUTH] && attempted[WEST])
		{
			trace.pop();
			if(trace.size() != 0){
				loc_x = trace.top()->x;
				loc_y = trace.top()->y;
			}
			for(unsigned i = 0; i < 4; i++)
				attempted[i] = false;
		}
	}
}

MazeNode* Maze::instantiateMazeNode(unsigned x, unsigned y)
{
	if(_maze[x][y] != 0)
		return 0;

	_maze[x][y] = new MazeNode;
	for(unsigned i = 0; i < 4; i++)
		_maze[x][y]->link[i] = 0;

	_maze[x][y]->x = x;
	_maze[x][y]->y = y;

	return _maze[x][y];
}

bool Maze::isNodeAt(int x, int y)
{
	if(x < 0 || y < 0 || x >= (int) _maze_w || y >= (int) _maze_h)
	{
		return true;
	}

	return (_maze[x][y] != 0);
}


void Maze::link(MazeNode* m1, MazeNode* m2, MazeNodeDirection dir)
{
	switch(dir)
	{
	case NORTH:
		m1->link[NORTH] = m2;
		m2->link[SOUTH] = m1;
		break;
	case EAST:
		m1->link[EAST] = m2;
		m2->link[WEST] = m1;
		break;
	case SOUTH:
		m1->link[SOUTH] = m2;
		m2->link[NORTH] = m1;
		break;
	case WEST:
		m1->link[WEST] = m2;
		m2->link[EAST] = m1;
		break;
	}
}

void Maze::handleEvents(sf::Event& e)
{
	if(e.type == sf::Event::KeyPressed)
	{
		// Speed up the simulation
		if(e.key.code == sf::Keyboard::Down)
		{
			if(_mouse_update_ms / 2 > 0)
				_mouse_update_ms /= 2;
		}

		// Slow down the simulation
		if(e.key.code == sf::Keyboard::Up)
		{
			if(_mouse_update_ms*2 != 0)
			 _mouse_update_ms *= 2;
		}
	}
}

void Maze::drawMaze(sf::RenderWindow* surf)
{
	if(_sprite_updated != true)
	{
		sf::Clock timer;

		for(unsigned i = 0; i < _maze_w; i++)
		{
			for(unsigned j = 0; j < _maze_h; j++)
			{
				std::vector<sf::RectangleShape> lines;
				unsigned x = _maze[i][j]->x;
				unsigned y = _maze[i][j]->y;

				if(_maze[i][j]->link[SOUTH] == 0)
				{
					sf::RectangleShape line(sf::Vector2f(_draw_size, 1));
					line.setPosition(x * _draw_size, y * _draw_size + _draw_size);
					lines.push_back(line);
				}
				if(_maze[i][j]->link[NORTH] == 0)
				{
					sf::RectangleShape line(sf::Vector2f(_draw_size, 1));
					line.setPosition(x * _draw_size, y * _draw_size);
					lines.push_back(line);
				}
				if(_maze[i][j]->link[EAST] == 0)
				{
					sf::RectangleShape line(sf::Vector2f(1, _draw_size));
					line.setPosition(x * _draw_size + _draw_size, y * _draw_size);
					lines.push_back(line);
				}
				if(_maze[i][j]->link[WEST] == 0)
				{
					sf::RectangleShape line(sf::Vector2f(1, _draw_size));
					line.setPosition(x * _draw_size, y * _draw_size);
					lines.push_back(line);
				}

				for(unsigned l = 0; l < lines.size(); l++)
					_texture->draw(lines[l]);
			}
		}

		sf::RectangleShape box(sf::Vector2f(_maze_w * _draw_size-2, _maze_h * _draw_size-2));
		box.setPosition(1,1);
		box.setFillColor(sf::Color::Transparent);
		box.setOutlineColor(sf::Color(120,120,120));
		box.setOutlineThickness(1);

		_texture->draw(box);

		_texture->display();
		_sprite.setTexture(_texture->getTexture());
		_sprite.setColor(sf::Color(120,120,120));
		_sprite_updated = true;

		std::cout << "Rendering complete (" << timer.getElapsedTime().asMilliseconds() << "ms.)" << std::endl;
	}


	surf->draw(_sprite);

	for(unsigned m = 0; m < _mice.size(); m++)
	{
		unsigned x = _mice[m]->_loc->x;
		unsigned y = _mice[m]->_loc->y;

		sf::RectangleShape cursor(sf::Vector2f(_draw_size-10, _draw_size-10));
		cursor.setPosition(x * _draw_size+5, y * _draw_size+5);
		cursor.setFillColor(_mice[m]->_color);
		cursor.setOutlineColor(sf::Color::Black);
		cursor.setOutlineThickness(1);

		surf->draw(cursor);
	}

	drawMiceList(surf);

	std::stringstream simspeed;
	simspeed << _mouse_update_ms;
	std::string simspeed_str = "Simulation speed = ";
	simspeed_str += simspeed.str() + "ms between ticks";
	sf::Text txt_simulation_speed(simspeed_str, _font, 12);
	txt_simulation_speed.setColor(sf::Color(80,80,80));
	txt_simulation_speed.setPosition(_maze_w*_draw_size-350, _maze_h*_draw_size+20);
	surf->draw(txt_simulation_speed);
}

void Maze::drawMiceList(sf::RenderWindow* surf)
{
	unsigned offset_y = _maze_h * _draw_size + _draw_size;
	unsigned offset_x = _draw_size;
	unsigned w = _maze_w * _draw_size - _draw_size * 2;
	unsigned h = 160;

	sf::RectangleShape box(sf::Vector2f(w, h));
	box.setFillColor(sf::Color::White);
	box.setOutlineColor(sf::Color(120,120,120));
	box.setOutlineThickness(1);
	box.setPosition(sf::Vector2f(offset_x, offset_y));

	sf::Text txt_mice_header("Current Mice: ", _font, 18);
	txt_mice_header.setColor(sf::Color(80,80,80));
	txt_mice_header.setPosition(offset_x + 5, offset_y + 5);

	sf::Text txt_resource_header("Resources:", _font, 12);
	txt_resource_header.setColor(sf::Color(80,80,80));
	txt_resource_header.setPosition(txt_mice_header.getPosition().x + 200, txt_mice_header.getPosition().y + 18);

	surf->draw(box);
	surf->draw(txt_mice_header);
	surf->draw(txt_resource_header);

	int row = 0;
	for(unsigned i = 0; i < _mice.size(); i++)
	{
		sf::RectangleShape color_rect(sf::Vector2f(10,10));
		color_rect.setPosition(offset_x + row * 50 + 20, offset_y + (i) * 20 + 40);
		color_rect.setFillColor(_mice[i]->_color);
		color_rect.setOutlineColor(sf::Color::Black);
		color_rect.setOutlineThickness(1);

		sf::Text mouse_name(_mice[i]->_name, _font, 12);
		mouse_name.setColor(sf::Color(80,80,80));
		mouse_name.setPosition(color_rect.getPosition().x + 15, color_rect.getPosition().y - 3);

		std::stringstream ss;
		ss << _mice[i]->_resources;
		sf::Text mouse_resources(ss.str(), _font, 12);
		mouse_resources.setColor(_mice[i]->_color);
		mouse_resources.setPosition(txt_resource_header.getPosition().x, mouse_name.getPosition().y);

		surf->draw(color_rect);
		surf->draw(mouse_name);
		surf->draw(mouse_resources);
	}
}

void Maze::updateMice()
{
	if(_update_timer.getElapsedTime().asMilliseconds() < _mouse_update_ms)
		return;
	else
		_update_timer.restart();

	for(unsigned m = 0; m < _mice.size(); m++)
	{
		_mice[m]->_resources++; // Each mouse gets one additional resource per tick.
		_mice[m]->_logic(_mice[m]);
	}
}

void Maze::registerMouse(void (*ai)(Mouse* me), std::string name, sf::Color color)
{
	if(ai == 0)
		return;

	Mouse* m = new Mouse();
	m->_loc = _maze[0][0];
	m->_logic = ai;
	m->_name = name;
	m->_color = color;
	m->_resources = 0;
	_mice.push_back(m);
}