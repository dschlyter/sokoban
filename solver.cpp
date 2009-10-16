#include <queue>
#include "solver.h"
#include "map.h"
#include "state.h"
#include <cstdlib>
#include <cstring>
#include <set>
#include "balancedtree.h"
#include "bucketlist.h"


bool compareStates::operator() (const intStatePair & left, const intStatePair & right) const {
	return (left.first>right.first);
};

char* Solver::solve(char* map) {
	
	this->noExpandedNodes = 0;

	Map gameMap = Map(map);
	//cout << gameMap.width() << " " << gameMap.height() << endl;
	Coordinate normalizedStartPos = gameMap.calcNormalizedPosition();

	int height = gameMap.height()*gameMap.width();
    State::initZobristHash(gameMap.width(), gameMap.height());
	State initialState = State(normalizedStartPos, gameMap.getBoxes(), gameMap.getStart());
	parentStates.insert(psMap(initialState.getHash(),parentState(0,stateMove(initialState.getMoveLoc(),initialState.getMoveType()))));
	
	TStorage<int, intStatePair> *q = new TStorage<int, intStatePair>();
	//BucketList *q = new BucketList(-50, 200, 200);
	//priority_queue<intStatePair, vector<intStatePair>, compareStates> *q = new priority_queue<intStatePair, vector<intStatePair>, compareStates>();

	//q->push(heuristic(initialState, gameMap), initialState);
	q->push(intStatePair(heuristic(initialState, gameMap), initialState));
	
	bool win = true;
	State * winningState = 0;
	while (q->size() > 0) {
		//State &tmp = q->pop();
		State tmp = (q->top()).second;
		q->pop();
		
		this->noExpandedNodes++;
		//cout << "poped state with cost: " << (q->top()).first << endl;
		//printState(tmp, gameMap);
		//cout << tmp.getHistory().size() << endl;
		
		vector<Coordinate> boxes = tmp.getBoxes();
		
		win = true;
		for (size_t i = 0; i < boxes.size(); i++) {
			if (!gameMap.isGoal(boxes[i]))
			{
				win = false;
				break;
			}
		}
		if (win) {
			winningState = new State(tmp);
			break;
		}

        /*cerr << "======================================" << endl << endl;
        cerr << "Expanding:" << endl << endl;
        gameMap.printState(tmp);
        cerr << "Generated:" << endl << endl;*/
		vector<State> newStates = gameMap.getSuccessorStates(tmp);
		for (size_t i = 0; i < newStates.size(); i++) {
            //gameMap.printState(newStates[i]);
			
			State state = newStates[i];

			if (!(this->parentStates.insert(psMap(state.getHash(),parentState(tmp.getHash(),stateMove(state.getMoveLoc(),state.getMoveType())))).second)) {
				// State already visited: skip
				continue;
			}
			
		int score = heuristic(state, gameMap);
			//cout << "Pushed new state.. with cost: " << score << endl;
			intStatePair tmp = intStatePair(score, state);
			q->push(tmp);
			//q->push(score, state);
		}
	}

	if (win) {
		//cout << winningState->getHistory() << endl;
        cout << "Solution found!" << endl << endl;
        gameMap.printState(*winningState);
		cout << "No of expanded nodes: " << noExpandedNodes << endl;
        string history = gameMap.backtrack(winningState, &parentStates);
        cout << "Solution: " << history << endl;
		char * ret = new char[history.size()+5];
		strcpy(ret, history.c_str());
		return ret;
	} else {
        cout << "Solution not found!" << endl;
		cout << "No of expanded nodes:" << noExpandedNodes << endl;
	    return "FAIL";
    }
	
	delete q;
}

int Solver::heuristic(State state, Map map) {
	vector<Coordinate> boxes = state.getBoxes();
	vector<Coordinate> goals = map.getGoals();
	Coordinate player = state.getPlayerPosition();

	//TODO opt, 5 verkar bra på nr 2
	//eller nu verkar 3 och 2 bra xD
	int sum = 0;//state.getCost() / 3;


	int cc = 0;
    	int box_min = 100000000;
	for (size_t i = 0; i < boxes.size(); i++) {
	
		//good is a box is near a goal
		int min = 10000000;
		for (size_t j = 0; j < goals.size(); j++) {
			int tmp = manhattanDistance(boxes[i], goals[j]);
			if (tmp < min) {
				min = tmp;
			}
		}
		sum += min;
		//if(min < box_min) cc = i;
		
        /*
         * New search algorithm does not store player state
		//punicshes the player to go away from a blocking box.
		if(!map.isGoal(boxes[i])){
			if(	map.isWall(Coordinate(boxes[i].first+1,boxes[i].second)) && 
				map.isWall(Coordinate(boxes[i].first-1,boxes[i].second)) && 
				!map.isWall(Coordinate(boxes[i].first,boxes[i].second+1)) && 
				!map.isWall(Coordinate(boxes[i].first,boxes[i].second-1))){
				sum += manhattanDistance(player, boxes[i])*2;
			}else
			if(	map.isWall(Coordinate(boxes[i].first,boxes[i].second+1)) && 
				map.isWall(Coordinate(boxes[i].first,boxes[i].second-1)) && 
				!map.isWall(Coordinate(boxes[i].first+1,boxes[i].second)) && 
				!map.isWall(Coordinate(boxes[i].first-1,boxes[i].second))){
				sum += manhattanDistance(player, boxes[i])*2;
			}
		}*/
		
		//good if box is in a deadlock and goal.
        	//TODO expand for goal packing opt
		if(map.isGoal(boxes[i]) && map.isDeadLock(boxes[i])){
			sum -= boxes.size();
		}
	}
	//Wtf, increasing when far from goal?
	//sum -= manhattanDistance(player, boxes[cc]);

	return sum;

}

int Solver::manhattanDistance(Coordinate first, Coordinate second) {
	return abs(first.first - second.first) + abs(first.second - second.second);	
}


