#include <iostream>
#include <unistd.h>
#include "workerthread.h"
#include <ctime>
#include <cerrno>
#include <map>
#include <cstring>

using namespace std;

WorkerThread::WorkerThread(Solver * solver, int number) {
	this->solver = solver;
	this->number = number;
}

void WorkerThread::run() {
	bool win = true;
	State * winningState = 0;

	while (true) {
		
		toPushParents.clear();
		toPushQueue.clear();

		//cout << "Thread " << number << ": MWAHA, MY NODES!" << endl << flush;		
		int i = 0;
		while (i < solver->chunksize && solver->queue->size() > 0)  {

			State state = (solver->queue->pop()).second;

			vector<Coordinate> boxes = state.getBoxes();
			win = true;
			for (size_t j = 0; j < boxes.size(); j++) {
				if (!solver->gameMap->isGoal(boxes[j]))
				{
					win = false;
					break;
				}
			}
			if (win) {
				solver->isDone = true;
				//solver->gameMap->printState(state);
				winningState = new State(state);
				break;
			}

			vector<State> newStates = solver->gameMap->getSuccessorStates(state);

			for (size_t j = 0; j < newStates.size(); j++) {
				State stateChild = newStates[j];

				toPushParents.push_back(psMap(stateChild.getHash(), parentState(state.getHash(), stateMove(stateChild.getMoveLoc(), stateChild.getMoveType()))));

				int heur = solver->heuristic(stateChild, solver->gameMap);

				pair<int, State> pa(heur, stateChild);
				toPushQueue.push_back(pa);
			}
			i++;
		}

		if (win) {
			break;
		}


		for (size_t i = 0; i < toPushParents.size(); i++) {

			if (!(solver->parentStates.insert(toPushParents[i]).second))
				continue;
			solver->queue->push(toPushQueue[i]);
		}
		//solver->printHeuristics();
	}
	if (win) {

		//cout << "Solution found!" << endl << endl;
	    //cout << "Total number of expanded nodes: " << solver->noExpandedNodes << endl;
		//solver->gameMap->printState(*winningState);
		string history = solver->gameMap->backtrack(winningState, &(solver->parentStates));
		//cout << "Solution: " << history << endl;
		solver->solution = new char[history.size()+5];
		strcpy(solver->solution, history.c_str());

		
		delete winningState;

		//return "D";
		//cout << "Thread " << number << " found the solution: " << winningState << endl << flush;
	}
}

