/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   jackThread.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月11日, 下午8:37
 */

#include <unistd.h>
#include <stdio.h>
#include <iostream>

#include "jackThread.h"

////////////////
int THREAD_COUNT = 0;

////////////////
#ifdef __LINUX
void* func_thread(void* arg)
{
    JackThread* _thread = (JackThread*)arg;
    _thread->process();
    _thread->end();
}
#endif

#ifdef __C11
void JackThread::func_thread()
{
    process();
    end();
}
#endif

JackThread::JackThread(JACK_FUNC_THREAD _fProcess, JACK_FUNC_THREAD _fEnd)
{
    m_funcProcess = _fProcess;
    m_funcEnd     = _fEnd;    
}

JackThread::~JackThread()
{
    
}

void JackThread::setFunc(JACK_FUNC_THREAD _fProcess, JACK_FUNC_THREAD _fEnd)
{
    m_funcProcess = _fProcess;
    m_funcEnd     = _fEnd;    
}

void JackThread::begin()
{
    m_thread_status = 0;
    
#ifdef __LINUX
    
    pthread_create(
            &m_threadID,
            NULL,
            &func_thread,
            this
        );

#endif
    
#ifdef __C11
    
    std::thread _thread(&JackThread::func_thread, this);
    _thread.detach();
    //_thread.join();
    m_thread = &_thread;
    
#endif
    
    THREAD_COUNT += 1;
    
    
}

void JackThread::process()
{
    if( m_funcProcess )
    {
        m_funcProcess();        
    }       
    
    m_thread_status = 1;    

}

void JackThread::end()
{
    if( m_funcEnd )
    {
        m_funcEnd();        
    }
    
    m_thread_status = 2;
    
    THREAD_COUNT -= 1;

    std::cout << "thread is exit" << std::endl;
            
}

////////////////
#ifdef __LINUX
void* func_cys_thread(void* arg)
{
    JackThreadCys* _thread = (JackThreadCys*)arg;
    _thread->process();
    _thread->end();
}
#endif

#ifdef __C11
void JackThreadCys::func_cys_thread()
{
    process();
    end();
}
#endif

JackThreadCys::JackThreadCys(JACK_FUNC_THREAD _fProcess, JACK_FUNC_THREAD _fEnd):
JackThread::JackThread( _fProcess, _fEnd )
{
    
}

JackThreadCys::~JackThreadCys()
{
    
}

void JackThreadCys::begin()
{        
    m_thread_status = 0;
    
#ifdef __LINUX
    
    pthread_create(
            &m_threadID,
            NULL,
            &func_cys_thread,
            this
        );
#endif    
    
#ifdef __C11
    
    std::thread _thread(&JackThreadCys::func_cys_thread, this);
    //_thread.join();
    _thread.detach();
    m_thread = &_thread;
    
#endif

    THREAD_COUNT += 1;    
    
}

void JackThreadCys::process()
{
    while( m_thread_status == 0 )
    {
        if( m_funcProcess )
        {
            m_funcProcess();        
        }        
        
        sleep(1);
    }    
}

void JackThreadCys::stop(bool wait)
{
    m_thread_status = 1;    
    THREAD_COUNT -= 1;
    
    if( wait )
    {
        while( m_thread_status != 2 )
        {
            sleep(1);
        }
    }
}