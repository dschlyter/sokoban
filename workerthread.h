#ifndef WORKERTHREAD
#define WORKERTHREAD
#include "thread.h"
#include "solver.h"

class WorkerThread : public Thread {
	public:
		WorkerThread(Solver *, int);
		void run();

	private:
		Solver * solver;
		int number;
		int expandedNodes;

		State * states;
		//set<U64> hashes;
		vector<psMap> toPushParents;
		vector<intStatePair> toPushQueue;
};
#endif

