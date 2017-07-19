/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   coreUtil.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月13日, 下午1:58
 */

#include "coreUtil.h"
#include <sys/time.h>
#include <unistd.h>

USING_NS_CC;

void JackLabelAtlas::setIgnoreContentScaleFactorEx(bool bIgnoreContentScaleFactor)
{
    cocos2d::LabelAtlas::setIgnoreContentScaleFactor(bIgnoreContentScaleFactor);
}

namespace Core
{
    double getCurrentSecond()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        
        double _sec = tv.tv_sec;
        double _usec =tv.tv_usec;
        
        double _result = _sec + _usec * 0.000001f;
        return _result;
    };
    
    double getCurrentMSecond()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        
        double _sec = tv.tv_sec;
        double _usec =tv.tv_usec;
        
        double _result = _sec * 1000.0f + _usec * 0.001f;
        return _result;        
    };
    
    double getCurrentUSecond()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        
        double _sec = tv.tv_sec;
        double _usec =tv.tv_usec;
        
        double _result = _sec * 1000000.0f + _usec;
        return _result;                
    };

    ////////
    int getRandValue()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        
        const int _flag1 = rand();
        const int _flag2 = (int)tv.tv_usec;
        
        return _flag1 + _flag2;        
    };
    
    int getRandValue(int max)
    {
        return getRandValue() % max;
    };

    ////////
    static std::string _CURRENT_PATH = "";
    std::string getCurrentPath()
    {
        if( _CURRENT_PATH != "" )
        {
            return _CURRENT_PATH;
        }
        
        std::string _path = "";
        char szWorkDir[256] = {0};
        if(!getcwd(szWorkDir, 256))
        {
        
        }
        else
        {
            _path = szWorkDir;
            _CURRENT_PATH = _path;
        }
        
        return _path;
    }

    std::string getWritablePath()
    {
        ////////
        return FileUtils::getInstance()->getWritablePath();
    };
    
};









