#ifndef WORKERTHREAD
#define WORKERTHREAD
#include "thread.h"
#include "solver.h"

class WorkerThread : public Thread {
	public:
		WorkerThread(Solver);
		void run();

	private:
		Solver solver;

};

#endif

