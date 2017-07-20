/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memPool.h"

////////////////
Locker::Locker()
{
#ifdef LINUX
    pthread_mutex_init(&_mutex, NULL);
#endif

#ifdef WINDOWS 
	InitializeCriticalSection(&_mutex);
#endif

}

Locker::~Locker()
{
#ifdef LINUX
    pthread_mutex_destroy(&_mutex);
#endif

#ifdef WINDOWS
	DeleteCriticalSection(&_mutex);
#endif
}

void Locker::lock()
{
#ifdef LINUX
    pthread_mutex_lock(&_mutex);
#endif

#ifdef WINDOWS
	EnterCriticalSection(&_mutex);
#endif
}

void Locker::unlock()
{
#ifdef LINUX
    pthread_mutex_unlock(&_mutex);
#endif

#ifdef WINDOWS
	LeaveCriticalSection(&_mutex);
#endif 
}

////////////////

