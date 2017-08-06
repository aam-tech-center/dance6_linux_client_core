/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   renderSprite.h
 * Author: Jack.L
 *
 * Created on 2017年7月10日, 下午1:30
 */

#ifndef RENDERSPRITE_H
#define RENDERSPRITE_H

#include "cocos2d.h"
#define MAX_TEXTURE_COUNT 8

class renderSprite :public cocos2d::Sprite
{

protected:
    
    cocos2d::CustomCommand       m_customCommand;
    
    cocos2d::Texture2D*          m_tex[MAX_TEXTURE_COUNT];
    GLuint                       m_texHandle[MAX_TEXTURE_COUNT];
    
    renderSprite();
    virtual ~renderSprite();
    
    virtual bool init();
    virtual void draw(cocos2d::Renderer* render, const cocos2d::Mat4& transform, uint32_t transformUpdated);
    void onDraw();
    
public:
    
    CREATE_FUNC(renderSprite);
    
    void setTexture(int _index, cocos2d::Texture2D* _tex);    

    
};

#endif /* RENDERSPRITE_H */

