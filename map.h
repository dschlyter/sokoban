#ifndef MAP
#define MAP

// ******* DEFINES ****************************
#define WALL 1
#define EMPTY 2
#define GOAL 3

#define EMPTY_CHAR '.'
#define DEADLOCK_CHAR ','
#define WALL_CHAR '#'
#define BOX_ON_GOAL_CHAR 'O'
#define GOAL_CHAR '/'
#define PLAYER_START_CHAR 'x'
#define BOX_CHAR 'o'
// ********************************************

#include "state.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Map {
	private:
		// the map that contains the static things in the world
		char *static_map;
		
		size_t map_width;
		size_t map_height;
		// the start pos of all the boxes
		vector<Coordinate> boxesStart;
		vector<Coordinate> goals;
		// start pos of the player
		Coordinate playersStart;
		bool *deadLock;
		
	public:
		Map(const string);
        Map(const Map &);
		~Map();
        Map & operator=(const Map &);
		
		Coordinate getStart() const;
		vector<Coordinate> getBoxes() const;
		vector<Coordinate> getGoals() const;
		size_t width() const;
		size_t height() const;
		
		bool isGoal(const Coordinate) const;		
		bool isWall(const Coordinate) const;		
		bool isDeadLock(const Coordinate) const;
		vector<State> getSuccessorStates(const State) const;
        void printState(const State & state) const;
		friend ostream& operator<<(ostream &out, const Map &a );
};

#endif

