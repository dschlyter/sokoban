#include <queue>
#include "solver.h"
#include "map.h"
#include "state.h"
#include <cstdlib>
#include <cstring>
#include <set>

#define U64 unsigned long long int

bool compareStates::operator() (const intStatePair & left, const intStatePair & right) const {
	return (left.first>right.first);
};


// Generate 64-bit random number (naively)
U64 Solver::rand64() {
	return rand() ^ ((U64) rand() << 15) ^ ((U64) rand() << 30) ^ ((U64) rand() << 45) ^ ((U64) rand() << 60);
}

U64 Solver::zobristHash(State state, int width, int secondStart) {
	U64 key = 0;

	// Hash in player into key
	Coordinate pos = state.getPlayerPosition();
	key ^= this->zobrist[pos.second*width+pos.first];

	// Hash in boxes into key
	vector<Coordinate> boxes = state.getBoxes();
	for (int i = 0; i < boxes.size(); i++) {
		key ^= this->zobrist[secondStart + boxes[i].second*width+boxes[i].first];	
	}
	return key;
}

char* Solver::solve(char* map) {
	
	this->noExpandedNodes = 0;

	Map gameMap = Map(map);
	//cout << gameMap.width() << " " << gameMap.height() << endl;
    Coordinate normalizedStartPos = gameMap.calcNormalizedPosition();
	State initialState = State(normalizedStartPos, gameMap.getBoxes());
	
	priority_queue<intStatePair, vector<intStatePair>, compareStates> *q = new priority_queue<intStatePair, vector<intStatePair>, compareStates>();

	q->push(intStatePair(heuristic(initialState, gameMap), initialState));


	int height = gameMap.height()*gameMap.width();
	int width = 2;
	int totalSize = height * width;
	
	this->zobrist = new U64[totalSize];

	for (int type = 0; type < 2; type++) {
		for (int coord = 0; coord < height; coord++) {
			this->zobrist[type*height+coord] = rand64();
		}
	}

	bool win = true;
	State * winningState = 0;
	while (q->size() > 0) {
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

        cerr << "======================================" << endl << endl;
        cerr << "Expanding:" << endl << endl;
        gameMap.printState(tmp);
        cerr << "Generated:" << endl << endl;
		vector<State> newStates = gameMap.getSuccessorStates(tmp);
		for (size_t i = 0; i < newStates.size(); i++) {
            gameMap.printState(newStates[i]);
			
			State state = newStates[i];
			U64 hashKey = zobristHash(state, gameMap.width(), gameMap.width()*gameMap.height());
		

			if (!(this->repeatedStates.insert(hashKey).second)) {
				// State already visited: skip
				continue;
			}
			
		int score = heuristic(state, gameMap);
			//cout << "Pushed new state.. with cost: " << score << endl;
			intStatePair tmp = intStatePair(score, state);
			q->push(tmp);
		}
	}

	if (win) {

		//cout << winningState->getHistory() << endl;
        cout << "Solution found!" << endl;
		cout << "No of expanded nodes:" << noExpandedNodes << endl;
        //TODO lägg till history-grejer
		//char * hej = new char[winningState->getHistory().size()+5];
		//strcpy(hej, winningState->getHistory().c_str());
		return "D";
	} else {
        cout << "Solution not found!" << endl;
		cout << "No of expanded nodes:" << noExpandedNodes << endl;
	    return "FAIL";
    }

}

int Solver::heuristic(State state, Map map) {
	vector<Coordinate> boxes = state.getBoxes();
	vector<Coordinate> goals = map.getGoals();
	Coordinate player = state.getPlayerPosition();

    //TODO opt, 5 verkar bra på nr 2
	int sum = state.getCost() / 5;

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
		/*if(map.isGoal(boxes[i]) && map.isDeadLock(boxes[i])){
			sum -= boxes.size();
		}*/
	}
    //Wtf, increasing when far from goal?
	//sum -= manhattanDistance(player, boxes[cc]);

	return sum;

}

int Solver::manhattanDistance(Coordinate first, Coordinate second) {
	return abs(first.first - second.first) + abs(first.second - second.second);	
}


