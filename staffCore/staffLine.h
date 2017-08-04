/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   staffLine.h
 * Author: Jack.L
 *
 * Created on 2017年7月26日, 下午6:55
 */

#ifndef STAFFLINE_H
#define STAFFLINE_H

#include "cocos2d.h"

class staffLine :public cocos2d::Sprite
{
protected:

    float m_heightY;
    float m_trackY[5];
    
public:

    CREATE_FUNC(staffLine);
    
    void initLine(int _width, int _height);
    
    float getTrackPositinY(float _index);

};

#endif /* STAFFLINE_H */

