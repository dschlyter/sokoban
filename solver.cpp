#include <queue>
#include "solver.h"
#include "map.h"
#include "state.h"
#include <cstdlib>
#include <cstring>
#include <set>
#include "bucketqueue.h"


bool compareStates::operator() (const intStatePair & left, const intStatePair & right) const {
	return (left.first>right.first);
};


Solver::Solver(int chunksize) {
	this->chunksize = chunksize;
}

Solver::~Solver() {
    if(this->gameMap) delete gameMap;
    if(this->queue) delete queue;
    if(this->reverseQueue) delete reverseQueue;
    if(this->solution) delete[] solution;

    State::disposeZobristHash();
}

/*
void Solver::printHeuristics() {
	TStorageNode<int, intStatePair> * node = queue->GetFirst();
	int count = 1;
	int current = node->GetKey();

	while (node->GetNext()) {
		if (node->GetKey() == current) {
			count++;
		} else {
			cout << current << ": " << count << endl << flush;
			count = 1;
			current = node->GetKey();
		}
		node = node->GetNext();
	}
}
*/

void Solver::init(char* map) {

	solution = 0;
	this->noExpandedNodes = 0;

	this->gameMap = new Map(map);
	//cout << gameMap.width() << " " << gameMap.height() << endl;
	Coordinate normalizedStartPos = gameMap->calcNormalizedPosition();

    State::initZobristHash(gameMap->width(), gameMap->height());
	State initialState = State(normalizedStartPos, gameMap->getBoxes(), gameMap->getStart());
	parentStates.insert(psMap(initialState.getHash(),parentState(0,stateMove(initialState.getMoveLoc(),initialState.getMoveType()))));
	
	queue = new BucketQueue(1);
	queue->push(intStatePair(heuristic(initialState, gameMap), initialState));
	
    reverseQueue = new BucketQueue(1);
    vector<State> goalStates = gameMap->getAllEndStates();
    for(size_t i=0; i<goalStates.size(); i++) {
        State goalState = goalStates[i];
        reverseParentStates.insert(psMap(goalState.getHash(),parentState(0,stateMove(goalState.getMoveLoc(), goalState.getMoveType()))));
        reverseQueue->push(intStatePair(reverseHeuristic(goalState, gameMap), goalState));
        possibleEndStates.insert(pair<U64,State>(goalState.getHash(), goalState));
    }
}	

void Solver::solve() {
	bool win = false;
	State * winningState = 0;

	while (!win && this->queue->size() > 0) {
        toPushQueue.clear();
        toPushParents.clear();

		int chunksLeft = this->chunksize;
		while (chunksLeft-- > 0 && this->queue->size() > 0)  {
			State state = (this->queue->pop()).second;

			vector<State> newStates = this->gameMap->getSuccessorStates(state);

			for (size_t j = 0; j < newStates.size(); j++) {
				State stateChild = newStates[j];

				toPushParents.push_back(psMap(stateChild.getHash(), parentState(state.getHash(), stateMove(stateChild.getMoveLoc(), stateChild.getMoveType()))));

				int heur = this->heuristic(stateChild, this->gameMap);

				pair<int, State> pa(heur, stateChild);
				toPushQueue.push_back(pa);
			}
		}

		for (size_t i = 0; i < toPushParents.size(); i++) {
			if (this->parentStates.insert(toPushParents[i]).second) {
			    this->queue->push(toPushQueue[i]);

                if(this->reverseParentStates.count(toPushParents[i].first)) {
                    this->isDone = true;
                    winningState = new State(toPushQueue[i].second);
                    win = true;
                    // TODO
                    // Check map, reverse until no more parents
                    
                    // Pick the winner from the endstate map
                    // Call backtrack
                    // refactor away break / boolean mess
                }
            }
		}

		//solver->printHeuristics();
        
        // TODO reverse batch
        // TODO reverse insert and check (do not check against forward)
	}

	if (win) {
		string history = this->gameMap->backtrack(winningState, &(this->parentStates));
		this->solution = new char[history.size()+5];
		strcpy(this->solution, history.c_str());

		delete winningState;
	}
}


int Solver::heuristic(State state, Map * map) {
	vector<Coordinate> boxes = state.getBoxes();
	vector<Coordinate> goals = map->getGoals();

	int sum = 0; //state.getCost();

	for (size_t i = 0; i < boxes.size(); i++) {
		
		sum += map->distanceGoal(boxes[i]);
		
        //good if box is in a deadlock and goal.
        //TODO expand for goal packing opt
        if(map->isGoal(boxes[i]) && map->isDeadLock(boxes[i])){
            sum -= boxes.size();
        }
    }
    //Wtf, increasing when far from goal?
    //sum -= manhattanDistance(player, boxes[cc]);

    return sum;
}

int Solver::reverseHeuristic(State state, Map * map) {
	vector<Coordinate> boxes = state.getBoxes();

	int sum = 0;

	for (size_t i = 0; i < boxes.size(); i++) {
		sum += map->distanceStart(boxes[i]);
	}

	return sum;
}

