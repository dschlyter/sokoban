#include <pthread.h>
#include "thread.h"


Thread::Thread(const Thread & no) {

}

Thread::Thread() {
}

Thread::~Thread() {
}

int Thread::start() {
	return pthread_create(&thread, NULL, Thread::thread_func, (void * ) this);
}

int Thread::wait() {
	return pthread_join (thread, NULL);
}

