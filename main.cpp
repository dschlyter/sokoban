#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "solver.h"

#define BUFFERSIZE 4096 
#define CHUNKSIZE 900

int main(int argc, char *argv[])
{
    char buffer[BUFFERSIZE];
    memset(buffer, 0, BUFFERSIZE);
	
	ssize_t n = fread(buffer, 1, BUFFERSIZE, stdin);

	Solver * solver = new Solver(CHUNKSIZE);
	solver->init(buffer);
    solver->solve();

	char *solution = solver->solution;
	delete solver;

	cout << solution << endl << flush;
    
#ifdef VERIFY_SOLUTION
    bool valid = solver->gameMap->verifySolution(solution);
    if(!valid) {
        cout << "Solution is invalid :(" << endl;
        return 1;
    } else {
        cout << "Solution verified successfully! :D" << endl;
    }
#endif

	return 0;
}
