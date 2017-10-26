#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>

class ThreadLock {
	pthread_mutex_t locker;

public:
	inline ThreadLock() {
		pthread_mutex_init(&locker, NULL);
	}

	inline ~ThreadLock() {
		pthread_mutex_destroy(&locker);
	}

	inline void lock() {
		pthread_mutex_lock(&locker);
	}

	inline void unlock() {
		pthread_mutex_unlock(&locker);
	}
};

typedef void *(*ThreadFuncT)(void *);

class Thread {
public:
	static void run(ThreadFuncT func, void* data) {
		pthread_t t;
		pthread_create(&t, NULL, func, data);
	}
};


#endif
