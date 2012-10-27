#include "LPRMutex.h"

LPRMutex::LPRMutex(void)
{
#ifdef WIN32
	InitializeCriticalSection(&mMutex);
#else
	pthread_mutex_init(&mMutex, NULL);
#endif
}

LPRMutex::~LPRMutex(void)
{
#ifdef WIN32
	DeleteCriticalSection(&mMutex);
#else
	pthread_mutex_destroy(&mMutex);
#endif
}

void LPRMutex::lock()
{
#ifdef WIN32
	EnterCriticalSection(&mMutex);
#else
	pthread_mutex_lock(&mMutex);
#endif
}

bool LPRMutex::tryLock()
{
#ifdef WIN32
	return TryEnterCriticalSection(&mMutex) == TRUE;
	//return true;
#else
	return 0 == pthread_mutex_trylock(&mMutex);
#endif
}

void LPRMutex::unlock()
{
#ifdef WIN32
	LeaveCriticalSection(&mMutex);
#else
	pthread_mutex_unlock(&mMutex);
#endif
}

LPRMutexLocker::LPRMutexLocker(LPRMutex *pMutex) : mMutexPtr(pMutex)
{
	mMutexPtr->lock();
}

LPRMutexLocker::~LPRMutexLocker()
{
	mMutexPtr->unlock();
}