/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   sysMonitor.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月11日, 下午7:45
 */

#include <iostream>
#include "sysMonitor.h"
#include "cocos2d.h"
USING_NS_CC;

////////
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#include "jackThread.h"
#include "coreUtil.h"

extern int THREAD_COUNT;

////////
const char* SYS_MONIT_NAME[] =
{
    "CPU TOTAL",
    "CPU PROCESS",
    "CPU PERCENT",
    "MEM",
    "V MEM",
    "THREAD COUNT"    
};

////////
#define VMRSS_LINE 17
#define VMSIZE_LINE 13
#define PROCESS_ITEM 14

typedef struct {
	unsigned long user;
	unsigned long nice;
	unsigned long system;
	unsigned long idle;
}Total_Cpu_Occupy_t;


typedef struct {
	unsigned int pid;
	unsigned long utime;  //user time
	unsigned long stime;  //kernel time
	unsigned long cutime; //all user time
        unsigned long cstime; //all dead time
}Proc_Cpu_Occupy_t;

//获取第N项开始的指针
const char* get_items(const char*buffer ,unsigned int item){
	
	const char *p =buffer;

	int len = strlen(buffer);
	int count = 0;
	
	for (int i=0; i<len;i++){
		if (' ' == *p){
			count ++;
			if(count == item -1){
				p++;
				break;
			}
		}
		p++;
	}

	return p;
}


//获取总的CPU时间
unsigned long get_cpu_total_occupy(){
	
	FILE *fd;
	char buff[1024]={0};
	Total_Cpu_Occupy_t t;

	fd =fopen("/proc/stat","r");
	if (nullptr == fd){
		return 0;
	}
		
	fgets(buff,sizeof(buff),fd);
	char name[64]={0};
	sscanf(buff,"%s %ld %ld %ld %ld",name,&t.user,&t.nice,&t.system,&t.idle);
	fclose(fd);
	
	return (t.user + t.nice + t.system + t.idle);
}


//获取进程的CPU时间
unsigned long get_cpu_proc_occupy(unsigned int pid){
	
	char file_name[64]={0};
	Proc_Cpu_Occupy_t t;
	FILE *fd;
	char line_buff[1024]={0};
	sprintf(file_name,"/proc/%d/stat",pid);
	
	fd = fopen(file_name,"r");
	if(nullptr == fd){
		return 0;
	}
	
	fgets(line_buff,sizeof(line_buff),fd);
	
	sscanf(line_buff,"%u",&t.pid);
	const char *q =get_items(line_buff,PROCESS_ITEM);
	sscanf(q,"%ld %ld %ld %ld",&t.utime,&t.stime,&t.cutime,&t.cstime);
	fclose(fd);
	
	return (t.utime + t.stime + t.cutime + t.cstime);
}


//获取CPU占用率
float get_proc_cpu(unsigned int pid){
 	
	unsigned long totalcputime1,totalcputime2;
	unsigned long procputime1,procputime2;
	
	totalcputime1=get_cpu_total_occupy();
	procputime1=get_cpu_proc_occupy(pid);

	usleep(200000);

	totalcputime2=get_cpu_total_occupy();
	procputime2=get_cpu_proc_occupy(pid);
	
	float pcpu = 0.0;
	if(0 != totalcputime2-totalcputime1){ 
		pcpu=100.0 * (procputime2-procputime1)/(totalcputime2-totalcputime1);
	}
	
	return pcpu;
}


//获取进程占用内存
unsigned int get_proc_mem(unsigned int pid){
	
	char file_name[64]={0};
	FILE *fd;
	char line_buff[512]={0};
	sprintf(file_name,"/proc/%d/status",pid);
	
	fd =fopen(file_name,"r");
	if(nullptr == fd){
		return 0;
	}
	
	char name[64];
	int vmrss;
	for (int i=0; i<VMRSS_LINE-1;i++){
		fgets(line_buff,sizeof(line_buff),fd);
	}
	
	fgets(line_buff,sizeof(line_buff),fd);
	sscanf(line_buff,"%s %d",name,&vmrss);
	fclose(fd);

	return vmrss;
}


//获取进程占用虚拟内存
unsigned int get_proc_virtualmem(unsigned int pid){
	
	char file_name[64]={0};
	FILE *fd;
	char line_buff[512]={0};
	sprintf(file_name,"/proc/%d/status",pid);
	
	fd =fopen(file_name,"r");
	if(nullptr == fd){
		return 0;
	}
	
	char name[64];
	int vmsize;
	for (int i=0; i<VMSIZE_LINE-1;i++){
		fgets(line_buff,sizeof(line_buff),fd);
	}
	
	fgets(line_buff,sizeof(line_buff),fd);
	sscanf(line_buff,"%s %d",name,&vmsize);
	fclose(fd);

	return vmsize;
}


//进程本身
int get_pid(const char* process_name, const char* user = nullptr)
{
	if(user == nullptr){
		user = getlogin();	
	}
	
	char cmd[512];
	if (user){
		sprintf(cmd, "pgrep %s -u %s", process_name, user);	
	}

	FILE *pstr = popen(cmd,"r");	
	
	if(pstr == nullptr){
		return 0;	
	}

	char buff[512];
	::memset(buff, 0, sizeof(buff));
	if(NULL == fgets(buff, 512, pstr)){
		return 0;
	}

	return atoi(buff);
}

////////
JackLabelAtlas* __MONITOR_LABEL[ESM_COUNT];

void render_init_monit(Texture2D* texture);
void render_sys_monit(Renderer* _renderer, const Mat4& identity);

sysMonitor::sysMonitor():
m_monitThread(NULL)
{    
    m_monitThread = new JackThreadCys();
}

sysMonitor::~sysMonitor() 
{
    delete m_monitThread;
        
}

bool sysMonitor::init()
{
    pid_t pid = getpid();  
    char strProcessPath[1024] = {0};  
    if(readlink("/proc/self/exe", strProcessPath,1024) <=0)  
    {  
            return false;  
    }  
  
    char *strProcessName = strrchr(strProcessPath, '/');  
  
    if(!strProcessName)  
    {  
            printf("当前进程ID：%d\n", pid);  
            printf("当前进程名：\n");  
            printf("当前进程路径：%s\n", strProcessPath);  
    }  
    else  
    {  
            printf("当前进程ID：%d\n", pid);  
            printf("当前进程名：%s\n", ++strProcessName);  
            printf("当前进程路径：%s\n", strProcessPath);  
    }         
    
    m_selfPID = pid;
    
    ////////
    cocos2d::__FUNC_INIT_MONITOR = &render_init_monit;
    cocos2d::__FUNC_RENDER_MONITOR = &render_sys_monit;

  
    return true; 
}

const std::string sysMonitor::get_sys_info(ENUM_SYS_MONITOR _type)
{
    char _info[1024];
    memset(_info, 0, sizeof(_info));
    
    switch(_type)
    {
        case ESM_TIME_CPU:
        {
            sprintf(_info, "%lu", get_cpu_total_occupy());
            break;
        }
        case ESM_TIME_CPU_PROCESS:
        {
            sprintf(_info, "%lu", get_cpu_proc_occupy(m_selfPID));
            break;            
        }
        case ESM_TIME_CPU_PERCENT:
        {
            sprintf(_info, "%.2f", get_proc_cpu(m_selfPID));            
            break;
        }
        case ESM_MEM_PROCESS:
        {
            sprintf(_info, "%ld", get_proc_mem(m_selfPID));                        
            break;
        }
        case ESM_VMEM_PROCESS:
        {
            sprintf(_info, "%ld", get_proc_virtualmem(m_selfPID));                        
            break;
        }
        case ESM_THREAD_COUNT:
        {
            sprintf(_info, "%d", THREAD_COUNT);                        
            break;
        }
    }
    
    return _info;
}

void sysMonitor::update()
{
    ////////
    char _strTemp[256];
    
    for( int i=0; i<ESM_COUNT; i++ )
    {
        m_infoValue[i] = get_sys_info((ENUM_SYS_MONITOR)i);        
    }        
}

void sysMonitor::monit_start()
{
    ////////
    m_monitThread->setFunc(std::bind(&sysMonitor::update, this), NULL);
    m_monitThread->begin();
}

void sysMonitor::monit_end()
{
    m_monitThread->stop(true);
    
    ////////
    for( int i=0; i<ESM_COUNT; i++ )
    {
        CC_SAFE_RELEASE_NULL(__MONITOR_LABEL[i]);        
    }
    
}

////////////////
void render_init_monit(Texture2D* texture)
{
    ////////
    const cocos2d::Size _screenSize = cocos2d::Director::getInstance()->getWinSize();

    for( int i=0; i<ESM_COUNT; i++ )
    {
        __MONITOR_LABEL[i] = JackLabelAtlas::create();                   
        __MONITOR_LABEL[i]->setIgnoreContentScaleFactorEx(true);
    
        __MONITOR_LABEL[i]->retain();    
        __MONITOR_LABEL[i]->initWithString("INFOSAMPLER", texture, 12, 32 , '.');    
        
        __MONITOR_LABEL[i]->setAnchorPoint(Vec2(0.0, 1.0));
        __MONITOR_LABEL[i]->setPosition(Vec2(0.0, _screenSize.height - i*10));        
        
        __MONITOR_LABEL[i]->setColor(ccColor3B(255,0,0));
        __MONITOR_LABEL[i]->setScale(0.5);
    }    

}

////////////////
void render_sys_monit(Renderer* _renderer, const Mat4& identity)
{
    ////////
    sysMonitor::Instance()->updateInfo();
    
    ////////
    for( int i=0; i<ESM_COUNT; i++ )
    {
        __MONITOR_LABEL[i]->visit(_renderer, identity, 0);        
    }
}

void sysMonitor::updateInfo()
{
    ////////
    char _strTemp[256];
    for( int i=0; i<ESM_COUNT; i++ )
    {
        memset(_strTemp, 0, sizeof(_strTemp));
        sprintf(_strTemp, "%s : %s", SYS_MONIT_NAME[i], m_infoValue[i].c_str());

        __MONITOR_LABEL[i]->setString(_strTemp);                
    }    

    ////////    
    //std::cout << "rand test:" << Core::getRandValue(100) << std::endl;

}
