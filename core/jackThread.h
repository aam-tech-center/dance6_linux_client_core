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

#include <pthread.h>
#include <functional>

typedef std::function<void(void)> JACK_FUNC_THREAD;

class JackThread
{
protected:
    JACK_FUNC_THREAD        m_funcProcess;
    JACK_FUNC_THREAD        m_funcEnd;
    
    pthread_t               m_threadID;
    int                     m_thread_status;

    
    virtual void process();
    void end();
    
    friend void* func_thread(void* arg);
    
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
    friend void* func_cys_thread(void* arg);
    
public:
    JackThreadCys(JACK_FUNC_THREAD _fProcess = NULL, JACK_FUNC_THREAD _fEnd = NULL);
    
    virtual ~JackThreadCys();
    virtual void begin();

    void stop(bool wait=false);
};


#endif /* JACKTHREAD_H */

