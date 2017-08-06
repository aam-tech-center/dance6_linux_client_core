/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   staffLine.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月26日, 下午6:55
 */

#include "staffLine.h"
USING_NS_CC;

////////
void staffLine::initLine(float _width, float _height)
{
    ////////
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    ////////
    m_heightY = _height / 1.0f;
    
    ////////
    RenderTexture* _renderTex = RenderTexture::create(
            _width, _height * 6
            );
    
    DrawNode* drawnode = DrawNode::create();
    
    for( int i=1; i<= 5; i++ )
    {
        m_trackY[i-1] = i * _height;        
        
        //drawnode->drawSegment(Vec2(0,m_trackY[i-1]),Vec2(_width,m_trackY[i-1]),0.25, Color4F(0.0,0.0,0.0,1.0));    
        drawnode->drawLine(Vec2(0,m_trackY[i-1]),Vec2(_width,m_trackY[i-1]), Color4F(0.0,0.0,0.0,1.0));
        
    }

    BlendFunc bl = { GL_ONE, GL_ONE };
    drawnode->setBlendFunc(bl);
       
    ////////
    _renderTex->begin();    
    
    drawnode->visit();        
        
    CHECK_GL_ERROR_DEBUG();
    
    _renderTex->end();
    
    ////////
    Texture2D* _texture = _renderTex->getSprite()->getTexture();
    this->initWithTexture(_texture);
    this->setFlipY(true);
    
    _renderTex->retain();
        
};

void staffLine::initLine(cocos2d::Texture2D* _tex, float _height)
{
    float _width = _tex->getContentSize().width;
    
    ////////
    const auto visibleSize = Director::getInstance()->getVisibleSize();
    const Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    ////////
    m_heightY = _height / 1.0f;
    
    ////////
    RenderTexture* _renderTex = RenderTexture::create(
            _width, _height * 6
            );
    
    Sprite* _spt[5] =
    {
        Sprite::createWithTexture(_tex),
        Sprite::createWithTexture(_tex),
        Sprite::createWithTexture(_tex),
        Sprite::createWithTexture(_tex),
        Sprite::createWithTexture(_tex)
    };
    
    for( int i=1; i<= 5; i++ )
    {
        m_trackY[i-1] = i * _height;        
        _spt[i-1]->setPosition(Vec2(_width/2, m_trackY[i-1]));
    }
       
    ////////
    _renderTex->begin();    
    
    for( int i=0; i< 5; i++ )
    {
        _spt[i]->visit();
    }    
        
    CHECK_GL_ERROR_DEBUG();
    
    _renderTex->end();
    
    ////////
    Texture2D* _texture = _renderTex->getSprite()->getTexture();
    this->initWithTexture(_texture);
    this->setFlipY(true);
    
    _renderTex->retain();
}

float staffLine::getTrackPositinY(float _index)
{
    float _value = _index * m_heightY;
    
    return _value;
}








