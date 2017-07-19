/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   coreUtil.h
 * Author: Jack.L
 *
 * Created on 2017年7月13日, 下午1:58
 */

#ifndef COREUTIL_H
#define COREUTIL_H

#include <string>

#include "cocos2d.h"
#include "json/rapidjson.h"

class JackLabelAtlas:public cocos2d::LabelAtlas
{
public:

    ////////
    void setIgnoreContentScaleFactorEx(bool bIgnoreContentScaleFactor);
    
    ////////
    CREATE_FUNC(JackLabelAtlas);
        
    
    
};

namespace Core
{
    //time
    double getCurrentSecond();
    double getCurrentMSecond();
    double getCurrentUSecond();

    //rand value
    int getRandValue();
    int getRandValue(int max);
    
    //current directory
    std::string getCurrentPath();
    std::string getWritablePath();

    
};



#endif /* COREUTIL_H */

