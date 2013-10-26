#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "solver.h"
#include "workerthread.h"

#define BUFFERSIZE 4096 
#define CHUNKSIZE 900


int main(int argc, char *argv[])
{
    char buffer[BUFFERSIZE];
	
	ssize_t n = fread(buffer, 1, BUFFERSIZE, stdin);


	Solver * solver = new Solver(CHUNKSIZE);
	solver->init(buffer);


	WorkerThread * worker = new WorkerThread(solver, 0);

	worker->run();

	char *MYSOL = solver->solution;
	delete solver;
	delete worker;

	cout << MYSOL << endl << flush;
	return 0;
}
