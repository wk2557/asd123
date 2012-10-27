#ifndef LPRMUTEX_H
#define LPRMUTEX_H 

#ifdef WIN32
#include <Windows.h>
#else
#include <pthread.h>
#endif

class LPRMutex
{
public:
	LPRMutex(void);
	~LPRMutex(void);
	void lock();
	bool tryLock();
	void unlock();
private:
#ifdef WIN32
	CRITICAL_SECTION mMutex;
#else
	pthread_mutex_t mMutex;
#endif
};

class LPRMutexLocker
{
public:
	LPRMutexLocker(LPRMutex *pMutex);
	~LPRMutexLocker();
private:
	LPRMutex *mMutexPtr;
};

#endif
