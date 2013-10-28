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

	//BucketList *q = new BucketList(-50, 200, 200);
	//priority_queue<intStatePair, vector<intStatePair>, compareStates> *q = new priority_queue<intStatePair, vector<intStatePair>, compareStates>();

	//q->push(heuristic(initialState, gameMap), initialState);
	queue->push(intStatePair(heuristic(initialState, gameMap), initialState));
	
}	

void Solver::solve() {
	bool win = true;
	State * winningState = 0;

	while (true) {
        toPushQueue.clear();
        toPushParents.clear();

		int i = 0;
		while (i < this->chunksize && this->queue->size() > 0)  {
			State state = (this->queue->pop()).second;

			vector<Coordinate> boxes = state.getBoxes();
			win = true;
			for (size_t j = 0; j < boxes.size(); j++) {
				if (!this->gameMap->isGoal(boxes[j]))
				{
					win = false;
					break;
				}
			}
			if (win) {
				this->isDone = true;
				//solver->gameMap->printState(state);
				winningState = new State(state);
				break;
			}

			vector<State> newStates = this->gameMap->getSuccessorStates(state);

			for (size_t j = 0; j < newStates.size(); j++) {
				State stateChild = newStates[j];

				toPushParents.push_back(psMap(stateChild.getHash(), parentState(state.getHash(), stateMove(stateChild.getMoveLoc(), stateChild.getMoveType()))));

				int heur = this->heuristic(stateChild, this->gameMap);

				pair<int, State> pa(heur, stateChild);
				toPushQueue.push_back(pa);
			}
			i++;
		}

		if (win) {
			break;
		}


		for (size_t i = 0; i < toPushParents.size(); i++) {

			if (!(this->parentStates.insert(toPushParents[i]).second))
				continue;
			this->queue->push(toPushQueue[i]);
		}
		//solver->printHeuristics();
	}
	if (win) {
		//cout << "Solution found!" << endl << endl;
	    //cout << "Total number of expanded nodes: " << solver->noExpandedNodes << endl;
		//solver->gameMap->printState(*winningState);
		string history = this->gameMap->backtrack(winningState, &(this->parentStates));
		//cout << "Solution: " << history << endl;
		this->solution = new char[history.size()+5];
		strcpy(this->solution, history.c_str());

		
		delete winningState;

		//return "D";
	}
}


int Solver::heuristic(State state, Map * map) {
	vector<Coordinate> boxes = state.getBoxes();
	vector<Coordinate> goals = map->getGoals();

	//TODO opt, 5 verkar bra på nr 2
	//eller nu verkar 3 och 2 bra xD
	int sum = 0; //state.getCost();

	for (size_t i = 0; i < boxes.size(); i++) {
		
		sum += map->distanceGoal(boxes[i]);
		
		//if(min < box_min) cc = i;
		
        /*
         * New search algorithm does not store player state
		//punicshes the player to go away from a blocking box.
		if(!map->isGoal(boxes[i])){
			if(	map->isWall(Coordinate(boxes[i].first+1,boxes[i].second)) && 
				map->isWall(Coordinate(boxes[i].first-1,boxes[i].second)) && 
				!map->isWall(Coordinate(boxes[i].first,boxes[i].second+1)) && 
				!map->isWall(Coordinate(boxes[i].first,boxes[i].second-1))){
				sum += manhattanDistance(player, boxes[i])*2;
			}else
			if(	map->isWall(Coordinate(boxes[i].first,boxes[i].second+1)) && 
				map->isWall(Coordinate(boxes[i].first,boxes[i].second-1)) && 
				!map->isWall(Coordinate(boxes[i].first+1,boxes[i].second)) && 
				!map->isWall(Coordinate(boxes[i].first-1,boxes[i].second))){
				sum += manhattanDistance(player, boxes[i])*2;
			}
		}*/
		
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

