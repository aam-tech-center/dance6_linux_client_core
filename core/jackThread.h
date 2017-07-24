/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   jackThread.h
 * Author: Jack.L
 *
 * Created on 2017年7月11日, 下午8:37
 */

#ifndef JACKTHREAD_H
#define JACKTHREAD_H

#include <functional>

//#define __LINUX
#define __C11

#ifdef __LINUX
#include <pthread.h>
#endif

#ifdef __C11
#include <thread>
#endif

typedef std::function<void(void)> JACK_FUNC_THREAD;

class JackThread
{
protected:
    JACK_FUNC_THREAD        m_funcProcess;
    JACK_FUNC_THREAD        m_funcEnd;
    
#ifdef __LINUX
    pthread_t               m_threadID;
    friend void* func_thread(void* arg);

#endif
    
#ifdef __C11
    std::thread*            m_thread;
    void func_thread();
#endif
    
    int                     m_thread_status;

    
    virtual void process();
    void end();
        
public:
    
    JackThread(JACK_FUNC_THREAD _fProcess, JACK_FUNC_THREAD _fEnd);
    virtual ~JackThread();    
    virtual void begin();
    
    void setFunc(JACK_FUNC_THREAD _fProcess, JACK_FUNC_THREAD _fEnd);

    
};

class JackThreadCys:public JackThread
{
protected:    
    
    virtual void process();
    
#ifdef __LINUX
    friend void* func_cys_thread(void* arg);
#endif
    
#ifdef __C11
    void func_cys_thread();
#endif
    
public:
    JackThreadCys(JACK_FUNC_THREAD _fProcess = NULL, JACK_FUNC_THREAD _fEnd = NULL);
    
    virtual ~JackThreadCys();
    virtual void begin();

    void stop(bool wait=false);
};


#endif /* JACKTHREAD_H */

