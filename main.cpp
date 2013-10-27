#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "solver.h"
#include "workerthread.h"

#define BUFFERSIZE 4096 
#define CHUNKSIZE 900

bool verifySolution = true;

int main(int argc, char *argv[])
{
    char buffer[BUFFERSIZE];
    memset(buffer, 0, BUFFERSIZE);
	
	ssize_t n = fread(buffer, 1, BUFFERSIZE, stdin);

	Solver * solver = new Solver(CHUNKSIZE);
	solver->init(buffer);

	WorkerThread * worker = new WorkerThread(solver, 0);
	worker->run();

	char *solution = solver->solution;
	delete solver;
	delete worker;

	cout << solution << endl << flush;
    
    if(verifySolution) {
        bool valid = solver->gameMap->verifySolution(solution);
        if(!valid) {
            cerr << "Solution is invalid :(" << endl;
            return 1;
        } else {
            cerr << "Solution verified successfully! :D" << endl;
        }
    }

	return 0;
}
