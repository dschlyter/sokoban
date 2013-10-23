#ifndef THREAD
#define THREAD
#include <pthread.h>
#include <cstdlib>
#include <memory>

class Thread
{
	private:
		pthread_t thread;

		Thread(const Thread &);
		static void * thread_func(void * d) {
			((Thread *)d)->run();
		}
		
	public:
		Thread();
		virtual ~Thread();
		virtual void run() = 0;

		int start();
		int wait();
};


typedef std::auto_ptr<Thread> ThreadPtr;

#endif

