/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "memPool.h"

////////////////
Locker::Locker()
{
#ifdef __LINUX
    pthread_mutex_init(&_mutex, NULL);
#endif

#ifdef __WINDOWS 
    InitializeCriticalSection(&_mutex);
#endif

}

Locker::~Locker()
{
#ifdef __LINUX
    pthread_mutex_destroy(&_mutex);
#endif

#ifdef __WINDOWS
    DeleteCriticalSection(&_mutex);
#endif
}

void Locker::lock()
{
#ifdef __LINUX
    pthread_mutex_lock(&_mutex);
#endif

#ifdef __WINDOWS	
    EnterCriticalSection(&_mutex);
#endif
        
#ifdef __C11        
    _mutex.lock();
#endif
}

void Locker::unlock()
{
#ifdef __LINUX
    pthread_mutex_unlock(&_mutex);
#endif

#ifdef __WINDOWS
    LeaveCriticalSection(&_mutex);
#endif 
        
#ifdef __C11
    _mutex.unlock();
#endif
}

////////////////

