#include "Maze.h"
#include "Mouse.h"

#include <stack>
#include <algorithm>
#include <vector>

/* Random mouse picks a random direction and attempts to go that way.
   It has absolutely no intelligence and is a barebone implementation of
   a mouse ai function. */

void random_mouse(Mouse* me)
{
  unsigned random_dir = rand()%4;
  me->moveDirection(random_dir);
}

/* This mouse (is not very successful), if colliding with a wall, changes 
   direction in a clockwise fashion, and attempts to move again. */

void clockwise_mouse(Mouse* me)
{
	static int last_dir = rand()%4;

	bool moved = false;
	while(moved == false)
	{
		if(me->_loc->link[last_dir] != 0)
		{
			me->_loc = me->_loc->link[last_dir];
			moved = true;
		}
		else
		{
			if(last_dir == 0)
				last_dir = 1;
			else if(last_dir == 1)
				last_dir = 2;
			else if(last_dir == 2)
				last_dir = 3;
			else
				last_dir = 0;
		}
	}
}

/* This is my favorite mouse. It's the best way for a human to solve a maze.
   The idea is to simply keep your right hand along the wall. When the wall turns, so
   do you. As long as there are no islands you will eventually reach the end of the maze. */
void hand_along_wall_mouse(Mouse* me)
{
	static int direction = -1; // -1 Indicates first run.
	static int pending_move = -1;

	if(pending_move != -1)
	{
		me->_loc = me->_loc->link[pending_move];
		pending_move = -1;
		return;
	}

	if(direction == -1)
	{ 
		// On the first run we'll iterate the
		// sides of the node to find a wall.
		for(unsigned i = 0; i < 4; i++)
		{
			if(me->_loc->link[i] == 0)
				direction = i;
		}

		if(direction == -1)
			return;
	}

	// Look to the right of where we're facing, this is our 'hand'.
	int hand = (direction + 1) % 4;

	bool moved = false;
	while(moved == false)
	{
		// If there is no link then our hand
		// is still on the wall. Attempt to move forward.
		if(me->_loc->link[hand] == 0)
		{
			// Forward isn't blocked, move forward.
			if(me->_loc->link[direction] != 0)
			{
				me->_loc = me->_loc->link[direction];
				moved = true;
			}
			else
			{
				// Forward is blocked, change our hand to our direction, and swivel CCW.
				hand = direction;
				direction = (hand - 1 + 4) % 4;
			}
		}
		else
		{
			// Our hand isn't touching a wall, this means that on our right
			// there is an open link. This creates two scenarios, one scenario
			// is that we move right into a node that is blocked to our right,
			// in which case we place our hand here. The other scenario is that
			// we move right and the node is not blocked where our hand would go,
			// in which case we need to swing about one more time to find the back
			// side of the wall we had original came from.
			direction = hand;
			me->_loc = me->_loc->link[direction];
			moved = true;

			// Move hand
			hand = (hand + 1) % 4;

			// If our hand is touching a wall, we're done, but if it's not
			// we need to move yet one more time. We will do this by storing
			// a pending move which will be processed next tick.
			if(me->_loc->link[hand] != 0)
			{
				pending_move = hand;
			}
		}
	}
}

/* Trace mouse uses a back-tracking method to search the entire
   maze. It starts off by picking a random direction, and if it cannot
   go that direction it makes a note of it. Every node it visits gets pushed
   onto a stack. Once it reaches a node where it can no longer make any moves 
   it pops the stack and attempts the directions it hasn't tried in the
   top node. */

void trace_mouse(Mouse* me)
{
  static std::stack<MazeNode*> _trace;
  static std::map<MazeNode*, int> _visited;

  static bool _first_run = true;
  static bool _attempted[4] = {false, false, false, false};

  if(_first_run)
  {
    _trace.push(me->_loc);
    _first_run = false;
  }

  bool moved = false;
  while(moved == false)
  {
    if(_trace.size() > 0)
    {
      unsigned random_dir = rand()%4;
      if(_attempted[random_dir] == false)
      {
        _attempted[random_dir] = true;
        if(me->_loc->link[random_dir] == 0)
        {
          continue;
        }
        else
        {
          auto found = _visited.find(me->_loc->link[random_dir]);
          if(found != _visited.end())
          {
            // We've visited this node before, but maybe we 
            // haven't tried every direction.
            if(found->second < 16) 
            {
              moved = true;
              if(found->second & (1 << (random_dir+1)))
              {
                continue;
              }
              else
              {
                found->second |= (1 << (random_dir+1));
                me->_loc = me->_loc->link[random_dir];
                _trace.push(me->_loc);
              }
            }
          }
          else
          {
            // Brand new node, haven't visited this one before.
            moved = true;
            _visited[me->_loc->link[random_dir]] = 0;
            me->_loc = me->_loc->link[random_dir];
            _trace.push(me->_loc);
          }
        }
      }

      if(moved)
      {
        for(unsigned i = 0; i < 4; i++)
          _attempted[i] = false;
      }

      if(_attempted[0] && _attempted[1]  &&
         _attempted[2] && _attempted[3])
      {
        _trace.pop();
        me->_loc = _trace.top();
        for(unsigned i = 0; i < 4; i++)
          _attempted[i] = false;
      }
    }
    else
    {
      return;
    }
  }

  return;
}