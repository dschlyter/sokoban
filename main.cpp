#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "solver.h"

#define BUFFERSIZE 4096 
#define CHUNKSIZE 500
#define REVERSE_CHUNKSIZE 100

int main() // TODO remove (int argc, char *argv[])
{
    char buffer[BUFFERSIZE];
    memset(buffer, 0, BUFFERSIZE);
	
	fread(buffer, 1, BUFFERSIZE, stdin);

	Solver * solver = new Solver(CHUNKSIZE, REVERSE_CHUNKSIZE);
	solver->init(buffer);
    solver->solve();

	char *solution = solver->solution;
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

    delete solver;

	return 0;
}
