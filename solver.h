#ifndef SOLVER
#define SOLVER
#include <map>
#include "state.h"
#include "map.h"

using namespace std;

typedef pair<int, State> intStatePair;

class compareStates {
	public:
		bool operator() (const intStatePair &, const intStatePair &) const;
};

class Solver {
	private:
		void printState(State, Map);
		int manhattanDistance(Coordinate, Coordinate);

	public:
		char* solve(char*);
		int noExpandedNodes;
		int heuristic(State, Map);
		map<U64, parentState> parentStates;

};


#endif

