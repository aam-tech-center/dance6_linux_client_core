/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   memPool.h
 * Author: Jack.L
 *
 * Created on 2017年7月10日, 下午2:19
 */

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <pthread.h>
#include <bits/pthreadtypes.h>
#include <list>
using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) delete x;x=NULL;
#endif

#ifndef SAFE_DELETE[]
#define SAFE_DELETE[](x) delete[] x; x=NULL;
#endif

struct Locker
{
    pthread_mutex_t _mutex;
    
    Locker();    
    ~Locker();    
    void lock();    
    void unlock();
    
};

////////
template<class T>
void destroy_buff_list(std::list<T*>& _LIST)
{
    while( _LIST.size() > 0 )
    {
        T* _t = *_LIST.begin();
        
        _LIST.pop_front();
        delete _t;
    }
}

////////
template<class T>
struct MemPool
{    
    unsigned long long _BUFF_COUNT;
    typedef std::list<T*> BUFF_LIST;
    
    BUFF_LIST _unUsed;
    BUFF_LIST _inUsed;
    
    Locker    _lock;
    
    MemPool(unsigned long long _buffcount = 0xff):
    _BUFF_COUNT(_buffcount)
    {
        _lock.lock();
        
        for( unsigned long long i=0; i<_BUFF_COUNT; i++ )
        {
            _unUsed.push_back(new T);
        }
        
        _lock.unlock();
    };
    
    ~MemPool()
    {
        _lock.lock();
        
        destroy_buff_list<T>(_unUsed);
        destroy_buff_list<T>(_inUsed);
        
        _lock.unlock();
    };
    
    size_t buff_size()
    {
        return sizeof(T);
    };
    
    size_t buff_used_count()
    {
        return _inUsed.size();
    };
    
    size_t buff_unused_count()
    {
        return _unUsed.size();        
    };
    
    T* create()
    {
        T* t = NULL;
        
        ////////        
        _lock.lock();
        
        if( _unUsed.size() > 0 )
        {
            t = *_unUsed.begin();
            _unUsed.pop_front();
        }
        else
        {
            t = new T;
            _BUFF_COUNT += 1;
        }
        
        _inUsed.push_back(t);
        
        _lock.unlock();        
        
        return t;
    };
    
    void release(T* t)
    {
        T* _t = t;
        
        ////////        
        _lock.lock();
        
        _inUsed.remove(t);        
        _unUsed.push_back(_t);
        
        _lock.unlock();                
    };

    void clear_mem()
    {
        _lock.lock();
        _BUFF_COUNT -= _unUsed.size();
        destroy_buff_list<T>(_unUsed);        
        _lock.unlock();
    };
    
    
};

#endif /* MEMPOOL_H */

