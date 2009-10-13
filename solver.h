#ifndef SOLVER
#define SOLVER
#include "state.h"
#include "map.h"

using namespace std;


typedef pair<int, State> intStatePair;


class compareStates {
	public:
		bool operator() (const intStatePair &, const intStatePair &) const;
};


void printState(State, Map);
char* solve(char*);
int heuristic(State, Map);
int manhattanDistance(Coordinate, Coordinate);

#endif

