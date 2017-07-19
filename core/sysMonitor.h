/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sysMonitor.h
 * Author: Jack.L
 *
 * Created on 2017年7月11日, 下午7:45
 */

#ifndef SYSMONITOR_H
#define SYSMONITOR_H

#include <string>

class JackThreadCys;

enum ENUM_SYS_MONITOR
{
    ESM_TIME_CPU = 0,
    ESM_TIME_CPU_PROCESS,
    ESM_TIME_CPU_PERCENT,
    ESM_MEM_PROCESS,
    ESM_VMEM_PROCESS,
    
    ESM_THREAD_COUNT,
        
    ESM_COUNT,
};

class sysMonitor 
{
    
protected:
    
    unsigned int   m_selfPID;
    JackThreadCys* m_monitThread;
    
    std::string    m_infoValue[ESM_COUNT];
    
    sysMonitor();
    virtual ~sysMonitor();
    
    void update();
    
public:
    
    static sysMonitor* Instance()
    {
        static sysMonitor _sm;
        return &_sm;
    }
    
    bool init();
    
    const std::string get_sys_info(ENUM_SYS_MONITOR _type);
    
    void monit_start();
    void monit_end();
    
    void updateInfo();    
       
};

#endif /* SYSMONITOR_H */

