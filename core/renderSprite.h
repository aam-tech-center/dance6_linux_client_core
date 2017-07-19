/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   renderSprite.h
 * Author: meida
 *
 * Created on 2017年7月10日, 下午1:30
 */

#ifndef RENDERSPRITE_H
#define RENDERSPRITE_H

#include "cocos2d.h"

class renderSprite :public cocos2d::Sprite
{

protected:
    
    
    
    renderSprite();
    virtual ~renderSprite();
    
    virtual bool init();
    
    
public:
    
    CREATE_FUNC(renderSprite);
    
private:

};

#endif /* RENDERSPRITE_H */

