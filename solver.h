#ifndef SOLVER
#define SOLVER
#include <map>
#include <queue>
#include "state.h"
#include "map.h"
#include "bucketqueue.h"

using namespace std;

typedef pair<int, State> intStatePair;

class compareStates {
	public:
		bool operator() (const intStatePair &, const intStatePair &) const;
};

class Solver {
	private:
		void printState(State, Map);
		int height;
		int width;
		int totalSize;

	public:
		Solver(int);
		void init(char*);
		int noExpandedNodes;
		int heuristic(State, Map *);
		map<U64, parentState> parentStates;

		Map * gameMap;
		char * solution;
		bool isDone;
		int chunksize;
		
		//priority_queue<intStatePair, vector<intStatePair>, compareStates> * queue;
		void printHeuristics();
		BucketQueue * queue;
};

#endif

