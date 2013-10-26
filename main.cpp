#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "solver.h"
#include "workerthread.h"

#define BUFFERSIZE 10024

int main(int argc, char *argv[])
{
    if (argc < 2) {
       fprintf(stderr,"usage %s boardnum\n", argv[0]);
       exit(0);
    }

	int threads = 4;
	int chunksize = 300;
	if (argc > 2)
		threads = atoi(argv[2]);
	if (argc > 3) 
		chunksize = atoi(argv[3]);


    char buffer[BUFFERSIZE];
	
	ssize_t n = fread(buffer, 1, BUFFERSIZE, stdin);


	Solver * solver = new Solver(chunksize);
	solver->init(buffer);

	cout << "Running with " << threads << " threads," << endl;
	cout << "and chunksize: " << chunksize << endl << endl;


	ThreadPtr * threadArray = new ThreadPtr[threads];

	for (int i = 0; i < threads; i++) {
		threadArray[i] = ThreadPtr(new WorkerThread(solver, i));
		threadArray[i]->start();
		//cout << "Started thread: " << i << endl << flush;
	}
	
	for (int i = 0; i < threads; i++) {
		//cout << "Waiting for thread " << i << "..." << endl << flush;
		threadArray[i]->wait();
		//cout << "Thread " << i << " finished." << endl << flush;
	}

	char *MYSOL = solver->solution;
	delete[] threadArray;
	delete solver;

	cout << MYSOL << endl << flush;
}
