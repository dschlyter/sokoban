#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "solver.h"
#include "workerthread.h"

#define BUFFERSIZE 10024
#define THREADS 4
#define CHUNKSIZE 300


int main(int argc, char *argv[])
{
    char buffer[BUFFERSIZE];
	
	ssize_t n = fread(buffer, 1, BUFFERSIZE, stdin);


	Solver * solver = new Solver(CHUNKSIZE);
	solver->init(buffer);


	ThreadPtr * threadArray = new ThreadPtr[THREADS];

	for (int i = 0; i < THREADS; i++) {
		threadArray[i] = ThreadPtr(new WorkerThread(solver, i));
		threadArray[i]->start();
	}
	
	for (int i = 0; i < THREADS; i++) {
		threadArray[i]->wait();
	}

	char *MYSOL = solver->solution;
	delete[] threadArray;
	delete solver;

	cout << MYSOL << endl << flush;
	return 0;
}
