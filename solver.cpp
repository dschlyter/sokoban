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
	State initialState = State(gameMap.getStart(), gameMap.getBoxes());
	
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
		
		vector<Coordinate> goals = gameMap.getGoals();
		vector<Coordinate> boxes = tmp.getBoxes();
		
		win = true;
		for (size_t i = 0; i < goals.size(); i++) {
			bool success = false;
			for (size_t j = 0; j < boxes.size(); j++) {
				if (goals[i] == boxes[j]) {
					success = true;
					break;
				}
			}
			if (!success) {
				win = false;
				break;
			}
		}
		if (win) {
			winningState = new State(tmp);
			break;
		}

		vector<State> newStates = gameMap.getSuccessorStates(tmp);
		for (size_t i = 0; i < newStates.size(); i++) {
			
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

		cout << winningState->getHistory() << endl;
		cout << "No of expanded nodes:" << noExpandedNodes << endl;
		char * hej = new char[winningState->getHistory().size()+5];
		strcpy(hej, winningState->getHistory().c_str());
		return hej;
	}

	return "FAIL";
}

int Solver::heuristic(State state, Map map) {
	vector<Coordinate> goals = map.getGoals();
	vector<Coordinate> boxes = state.getBoxes();

	int sum = state.getCost();

	for (size_t i = 0; i < boxes.size(); i++) {
		int min = 10000000;
		for (size_t j = 0; j < goals.size(); j++) {
			int tmp = manhattanDistance(boxes[i], goals[j]);
			if (tmp < min) {
				min = tmp;
			}
		}
		sum += min;
	}
	return sum;	
}

int Solver::manhattanDistance(Coordinate first, Coordinate second) {
	return abs(first.first - second.first) + abs(first.second - second.second);	
}


