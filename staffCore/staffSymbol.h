/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   staffSymbol.h
 * Author: Jack.L
 *
 * Created on 2017年7月27日, 下午3:24
 */

#ifndef STAFFSYMBOL_H
#define STAFFSYMBOL_H

#include "cocos2d.h"

class staffSymbol:
public cocos2d::Sprite
{
    cocos2d::Texture2D* m_baseTex;
    
public:

    CREATE_FUNC(staffSymbol);

    void initSymbol(float _width, float _height, bool _solid);    
    void initSymbol(float _width, float _height, bool _solid, float _vline);
    void initSymbol(cocos2d::Texture2D* _tex, float _width, float _height, float _vline);
    

};

class staffSymbolEx:
public cocos2d::Sprite
{
public:
    
    CREATE_FUNC(staffSymbolEx);
    
    
};

class staffSymbolLine:
public cocos2d::Sprite
{
public:
    
    CREATE_FUNC(staffSymbolLine);
    
    void initSBLine(float _width, int _count);
};

#endif /* STAFFSYMBOL_H */

