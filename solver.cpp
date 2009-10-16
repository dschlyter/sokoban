#include <queue>
#include "solver.h"
#include "map.h"
#include "state.h"
#include <cstdlib>
#include <cstring>
#include <set>
#include "balancedtree.h"
#include "bucketlist.h"
#include <cmath>


bool compareStates::operator() (const intStatePair & left, const intStatePair & right) const {
	return (left.first>right.first);
};


Solver::Solver(int chunksize) {
	this->chunksize = chunksize;
}

void Solver::init(char* map) {

	pthread_mutex_init(&queueMutex, 0);
	pthread_mutex_init(&setMutex, 0);
	pthread_mutex_init(&winMutex, 0);
	pthread_mutex_init(&countMutex, 0);
	
	solution = 0;
	this->noExpandedNodes = 0;

	this->gameMap = new Map(map);
	//cout << gameMap.width() << " " << gameMap.height() << endl;
	Coordinate normalizedStartPos = gameMap->calcNormalizedPosition();

	int height = gameMap->height()*gameMap->width();
    State::initZobristHash(gameMap->width(), gameMap->height());
	State initialState = State(normalizedStartPos, gameMap->getBoxes(), gameMap->getStart());
	parentStates.insert(psMap(initialState.getHash(),parentState(0,stateMove(initialState.getMoveLoc(),initialState.getMoveType()))));
	
	queue = new TStorage<int, intStatePair>();
	//BucketList *q = new BucketList(-50, 200, 200);
	//priority_queue<intStatePair, vector<intStatePair>, compareStates> *q = new priority_queue<intStatePair, vector<intStatePair>, compareStates>();

	//q->push(heuristic(initialState, gameMap), initialState);
	queue->push(intStatePair(heuristic(initialState, gameMap), initialState));
	
}	


int Solver::heuristic(State state, Map * map) {
	vector<Coordinate> boxes = state.getBoxes();
	vector<Coordinate> goals = map->getGoals();
	Coordinate player = state.getPlayerPosition();

	//TODO opt, 5 verkar bra på nr 2
	//eller nu verkar 3 och 2 bra xD
	int sum = 0; //state.getCost();

    int goalSum[goals.size()];
    int goalMax = 0;
    int goalMaxi = 0;
	for (size_t i = 0; i < boxes.size(); i++) {
		//good is a box is near a goal
		int min = 10000000;
		for (size_t j = 0; j < goals.size(); j++) {
			int tmp = manhattanDistance(boxes[i], goals[j]);
            goalSum[j]+=tmp;
            if (goalSum[j] > goalMax){
                goalMax = goalSum[j];
                goalMaxi = j;
            }
			if (tmp < min) {
				min = tmp;
			}
		}
		sum += min;

		//good if box is in a deadlock and goal.
        //TODO expand for goal packing opt
		if(map->isGoal(boxes[i]) && map->isDeadLock(boxes[i])){
			sum -= boxes.size();
		}
	}

	for (size_t i = 0; i<boxes.size(); i++) {
        sum += manhattanDistance(boxes[i], goals[goalMaxi]);
    }

	return sum;
}

int Solver::manhattanDistance(Coordinate first, Coordinate second) {
	return abs(first.first - second.first) + abs(first.second - second.second);	
}


