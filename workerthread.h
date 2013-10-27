#ifndef WORKERTHREAD
#define WORKERTHREAD
#include "solver.h"

class WorkerThread {
	public:
		WorkerThread(Solver *, int);
		void run();

	private:
		Solver * solver;
		int number;

		//set<U64> hashes;
		vector<psMap> toPushParents;
		vector<intStatePair> toPushQueue;
};
#endif

