#ifndef SOLVER
#define SOLVER
#include <set>
#include "state.h"
#include "map.h"


#define U64 unsigned long long int

using namespace std;


typedef pair<int, State> intStatePair;



class compareStates {
	public:
		bool operator() (const intStatePair &, const intStatePair &) const;
};

class Solver {
	private:
		U64 rand64();
		U64 * zobrist;

		void printState(State, Map);
		int manhattanDistance(Coordinate, Coordinate);

	public:
		char* solve(char*);
		U64 zobristHash(State, int, int);
		int noExpandedNodes;
		int heuristic(State, Map);
		set<U64> repeatedStates;

};


#endif

