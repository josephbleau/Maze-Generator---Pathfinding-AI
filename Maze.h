#pragma once;

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

enum MazeNodeDirection
{
	NORTH = 0,
	EAST  = 1,
	SOUTH = 2,
	WEST  = 3
};

struct MazeNode
{
	MazeNode* link[4];
	unsigned x;
	unsigned y;
};


class Mouse;
class Maze
{
private:
	std::vector<std::vector<MazeNode*>> _maze;

	std::vector<Mouse*> _mice;
	unsigned _mouse_update_ms;

	unsigned _maze_w;
	unsigned _maze_h;

	int _draw_size;

	bool _sprite_updated;
	sf::Sprite _sprite;
	sf::RenderTexture* _texture;

	sf::Clock _update_timer;
	sf::Font _font;

	void generate();

	MazeNode* instantiateMazeNode(unsigned x, unsigned y);
	bool isNodeAt(int x, int y);
	void link(MazeNode* m1, MazeNode* m2, MazeNodeDirection dir);

public:
	Maze(unsigned w, unsigned h);
	~Maze();

	void handleEvents(sf::Event& e);

	void drawMaze(sf::RenderWindow* surf);
	void drawMiceList(sf::RenderWindow* surf);

	void updateMice();
	void registerMouse(void (*ai)(Mouse* me), std::string name = "Unnamed", sf::Color = sf::Color(120,120,120));
};
