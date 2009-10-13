#include <queue>
#include "solver.h"
#include "map.h"
#include "state.h"
#include <cstdlib>
#include <cstring>

bool compareStates::operator() (const intStatePair & left, const intStatePair & right) const {
	return (left.first<right.first);
};


void printState(State state, Map map) {
	for (int x = 0; x < map.width(); x++) {
		for (int y = 0; y < map.height(); y++) {
			Coordinate tmp;
			tmp.first = x;
			tmp.second = y;

			if (map.isWall(tmp)) {
				cout << "#";
			}
			else if (state.getPlayerPosition() == tmp) {
				cout << "x";
			}
			else {
				vector<Coordinate> boxes = state.getBoxes();
				bool done = false;
				for (int i = 0; i < boxes.size(); i++) {
					if (boxes[i].first == x && boxes[i].second == y) {
						done = true;
						cout << "o";
						break;
					}
				}
				if (!done) {
					vector<Coordinate> goals = map.getGoals();
					for (int i = 0; i < goals.size(); i++) {
						if (goals[i].first == x && goals[i].second == y) {
							done = true;
							cout << "/";
							break;
						}
					}
					if (!done) {
						cout << ".";
					}
				}
			}
		}
		cout << endl;
	}

	for (int i = 0; i < 1000000; i++);


}

char* solve(char* map) {
	

	cout << "Start!" << endl;
	Map gameMap = Map(map);
	cout << "Alive" << endl;
	State initialState = State(gameMap.getStart(), gameMap.getBoxes());
	cout << "Alive" << endl;
	
	priority_queue<intStatePair, vector<intStatePair>, compareStates> *q = new priority_queue<intStatePair, vector<intStatePair>, compareStates>();
	cout << "Alive" << endl;

	q->push(intStatePair(heuristic(initialState, gameMap), initialState));
	cout << "Alive" << endl;

	cout << "Pushed initial state." << endl;
	bool win = true;
	State * winningState = 0;
	while (q->size() > 0) {
		State tmp = (q->top()).second;
		q->pop();
		cout << "Pop!" << endl;
		//printState(tmp, gameMap);
		
		vector<Coordinate> goals = gameMap.getGoals();
		vector<Coordinate> boxes = tmp.getBoxes();
		
		cout << "goalsize: " << goals.size() << endl << "boxesize: " << boxes.size() << endl;
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
		cout << "alive" << endl;
		if (win) {
			cout << "WIN!" << endl;
			winningState = new State(tmp);
			break;
		}

		cout << "alive" << endl;
		vector<State> newStates = gameMap.getSuccessorStates(tmp);
		cout << "alive" << endl;
		for (size_t i = 0; i < newStates.size(); i++) {
			q->push(intStatePair(heuristic(newStates[i], gameMap), newStates[i]));	
		}
		cout << "Pushed " << newStates.size() << " new states." << endl;
	}

	if (win) {
		char * hej = new char[winningState->getHistory().size()+5];
		strcpy(hej, winningState->getHistory().c_str());
		return hej;
	}

	return "FAIL";
}

int heuristic(State state, Map map) {
	cout << "Start heuristic" << endl;
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
	cout << "End heuristic" << endl;
	return sum;	
}

int manhattanDistance(Coordinate first, Coordinate second) {
	return abs(first.first - second.first) + abs(first.second - second.second);	
}


