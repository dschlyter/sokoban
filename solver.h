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
		U64 zobristHash(State, int, int);
		U64 * zobrist;
		set<U64> repeatedStates;
		
		void printState(State, Map);
		int heuristic(State, Map);
		int manhattanDistance(Coordinate, Coordinate);

	public:
		char* solve(char*);

};


#endif

