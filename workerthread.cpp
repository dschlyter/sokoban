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
	expandedNodes = 0;

	while (true) {
		
		this->states = new State[solver->chunksize];
		toPushParents.clear();
		toPushQueue.clear();

		if (solver->isDone) {
			//cout << "#####################" << endl << "THREAD " << number << " WON!" << endl << "#####################" << endl << flush;
			win = false;
			break;
		}

		if (solver->queue->size() == 0) {
			usleep(100000); // sleep 1ms
		} 
		else {
			//cout << "Thread " << number << ": MWAHA, MY NODES!" << endl << flush;		
			int i = 0;
			while (solver->queue->size() > 0 && i < solver->chunksize) {
				State state = (solver->queue->top()).second;
				solver->queue->pop();

				states[i] = state;
				i++;
			}
			//cout << "Thread " << number << ": I stole " << i << " nodes, " << solver->queue->size() << " left." << endl << flush;
			expandedNodes += i;

			solver->noExpandedNodes += i;
			//this->noExpandedNodes++;

			for (int s = 0; s < i; s++) {
				State state = states[s];
				//cout << "poped state with cost: " << (q->top()).first << endl;
				//printState(tmp, gameMap);
				//cout << tmp.getHistory().size() << endl;

				vector<Coordinate> boxes = state.getBoxes();
				win = true;
				for (size_t i = 0; i < boxes.size(); i++) {
					if (!solver->gameMap->isGoal(boxes[i]))
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
			
				for (size_t i = 0; i < newStates.size(); i++) {
					State stateChild = newStates[i];
					
					toPushParents.push_back(psMap(stateChild.getHash(), parentState(state.getHash(), stateMove(stateChild.getMoveLoc(), stateChild.getMoveType()))));

					int heur = solver->heuristic(stateChild, solver->gameMap);

					pair<int, State> pa(heur, stateChild);
					toPushQueue.push_back(pa);
				}
			}

			if (win) {
				break;
			}

			
			for (size_t i = 0; i < toPushParents.size(); i++) {
	
				if (!(solver->parentStates.insert(toPushParents[i]).second))
					continue;
				solver->queue->push(toPushQueue[i]);
			}

			delete[] states;
		}
	}
	if (win) {

		//cout << "Solution found!" << endl << endl;
	    //cout << "Total number of expanded nodes: " << solver->noExpandedNodes << endl;
		//solver->gameMap->printState(*winningState);
		string history = solver->gameMap->backtrack(winningState, &(solver->parentStates));
		//cout << "Solution: " << history << endl;
		solver->solution = new char[history.size()+5];
		strcpy(solver->solution, history.c_str());

		
		delete[] states;
		delete winningState;

		//return "D";
		//cout << "Thread " << number << " found the solution: " << winningState << endl << flush;
	}
}

