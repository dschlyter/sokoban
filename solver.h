#ifndef SOLVER
#define SOLVER
#include <unordered_map>
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
	public:
		Solver(int);
		void init(char*);
        void solve();
		void printHeuristics();

        // TODO gameMap writes out of bounds, find where and and fix
        double memoryOverwriteHere;
        double memoryOverwriteHere2;
		Map * gameMap;
		char * solution;

	private:
		void printState(State, Map);
		int heuristic(State, Map *);
		int reverseHeuristic(State, Map *);

		BucketQueue * queue;
		BucketQueue * reverseQueue;

		unordered_map<U64, parentState> parentStates;
		unordered_map<U64, parentState> reverseParentStates;
		unordered_map<U64, State> possibleEndStates;

        vector<psMap> toPushParents;
        vector<intStatePair> toPushQueue;

		int height;
		int width;
		int totalSize;

		bool isDone;
		int chunksize;
		int noExpandedNodes;
};

#endif

