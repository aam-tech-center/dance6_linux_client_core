/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   renderSprite.cpp
 * Author: meida
 * 
 * Created on 2017年7月10日, 下午1:30
 */

#include "renderSprite.h"
#include "appShader.h"

USING_NS_CC;

renderSprite::renderSprite():
Sprite::Sprite()
{
}

renderSprite::~renderSprite() {
}

bool renderSprite::init()
{
    ////////
    Texture2D* _tex = new Texture2D;
    
    ////////
    const int _width  = 512;
    const int _height = 512;
    
    unsigned char* _buffer = (unsigned char*)malloc(_width * _height * 4);
    Size _texSize = Size(_width, _height);
    
    Texture2D::PixelFormat _texFormat = Texture2D::PixelFormat::RGBA8888;
   
    
    _tex->initWithData(
            _buffer,
            _width * _height * 4,
            _texFormat,
            _width,
            _height,
            _texSize
            );
    
    
        
    ////////
    if( !Sprite::initWithTexture(_tex) )
    {
        ////////
        return false;        
    }
    
    ////////
    CCLog("Success123");
    
    GLProgram* _pgp =
    appShader::Instance()->getProgram(EPT_SINGLE_COLOR);
    
    this->setGLProgram(_pgp);
    
    this->retain();
    
    return true;
}

