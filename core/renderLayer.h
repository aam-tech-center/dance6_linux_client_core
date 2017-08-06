/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   renderLayer.h
 * Author: root
 *
 * Created on 2017年8月1日, 上午11:32
 */

#ifndef RENDERLAYER_H
#define RENDERLAYER_H

#include "cocos2d.h"

class renderLayer:
public cocos2d::Layer
{
protected:
    
    ////////
    cocos2d::CustomCommand       m_customCommand;    
    
    ////////   
    GLuint                       m_texID[2];
    GLuint                       m_texHandle[2];        
        
    ////////
    cocos2d::GLProgram*          m_program;
    
    ////////
    renderLayer();
    virtual ~renderLayer();
    
    virtual bool init();
    virtual void draw(cocos2d::Renderer* render, const cocos2d::Mat4& transform, uint32_t transformUpdated);
    void onDraw();
    
public:
    
    CREATE_FUNC(renderLayer);

    void setLayerGLProgram(cocos2d::GLProgram* _pgp);
    void setTextureID(GLuint _texid, int index);
    
};

#endif /* RENDERLAYER_H */

