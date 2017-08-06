/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   staffSymbol.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月27日, 下午3:24
 */

#include "staffSymbol.h"
#include <math.h>

USING_NS_CC;



////////
void staffSymbol::initSymbol(float _width, float _height, bool _solid)
{
    ////////        
    RenderTexture* _renderTex = RenderTexture::create(_width, _height);
    
    
    ////////
    DrawNode* drawnode = DrawNode::create();
        
    ////
    const float _R         = _height/2.0;
    const int vec_count    = 64;
    const float _cycWidth  = 0.6f;
    const float _alphaFlag = 0.95f;
    
    Vec2* SphereList = new Vec2[vec_count];
    
    for( int i=0; i<vec_count; i++ )
    {
        const float _radiu = 3.1415926535f * 2 / (float)vec_count;
        SphereList[i] = (Vec2(_R * cosf(i * _radiu), _R * sinf(i * _radiu)));
    }
    
    drawnode->drawPolygon(SphereList, vec_count, Color4F(0.0,0.0,0.0,_solid?_alphaFlag:0.0), _cycWidth, Color4F(0.0, 0.0, 0.0, _alphaFlag));
    
    delete[] SphereList;
    
    ////
    drawnode->setPosition(Vec2(_width/2.0, _height/2.0));
    drawnode->setScaleY(_height/ _width);    
    BlendFunc bl = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
    drawnode->setBlendFunc(bl);
       
    ////////
    _renderTex->begin();    
    
    drawnode->visit();        
        
    CHECK_GL_ERROR_DEBUG();
    
    _renderTex->end();
    
    ////////
    Texture2D* _texture = _renderTex->getSprite()->getTexture();
    _texture->setAntiAliasTexParameters();
    
    m_baseTex = _texture;
    
    this->initWithTexture(_texture);
    this->setFlipY(true);
    
    _renderTex->retain();
    
    this->setAnchorPoint(Vec2(1.0f, 0.5f));
    //this->setScale(0.5f);
    
    ////////
    this->setColor(Color3B(8,8,8));


}

void staffSymbol::initSymbol(float _width, float _height, bool _solid, float _vline)
{
    ////////        
    RenderTexture* _renderTex = RenderTexture::create(_width, _height + abs(_vline));
        
    ////////
    DrawNode* drawnode = DrawNode::create();
        
    ////
    const float _R         = _height/2.0;
    const int vec_count    = 64;
    const float _cycWidth  = 0.6f;
    const float _alphaFlag = 0.95f;
    
    Vec2* SphereList = new Vec2[vec_count];
    
    for( int i=0; i<vec_count; i++ )
    {
        const float _radiu = 3.1415926535f * 2 / (float)vec_count;
        SphereList[i] = (Vec2(_R * cosf(i * _radiu), _R * sinf(i * _radiu)));
        
        if( _vline < 0.0 )
        {
            SphereList[i].y += abs(_vline);
            SphereList[i].y += _R * 2;            
        }
    }
    
    drawnode->drawPolygon(SphereList, vec_count, Color4F(0.0,0.0,0.0,_solid?_alphaFlag:0.0), _cycWidth, Color4F(0.0, 0.0, 0.0, _alphaFlag));
    
    delete[] SphereList;
    
    if( _vline > 0.1 )
    {
        drawnode->drawSegment(Vec2(_R * 1, 0), Vec2(_R * 1, _vline), 0.5f, Color4F(0.0,0.0,0.0,1.0));
    }
    else if( _vline < -0.1 )
    {
        drawnode->drawSegment(Vec2(-_R * 1, _R * 2), Vec2(-_R * 1, _R * 2 -_vline), 0.5f, Color4F(0.0,0.0,0.0,1.0));        
    }
    
    ////
    drawnode->setPosition(Vec2(_width/2.0, _height/2.0));
    drawnode->setScaleY(_height/ _width);    
    BlendFunc bl = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};
    drawnode->setBlendFunc(bl);
       
    ////////
    _renderTex->begin();    
    
    drawnode->visit();        
        
    CHECK_GL_ERROR_DEBUG();
    
    _renderTex->end();
    
    ////////
    Texture2D* _texture = _renderTex->getSprite()->getTexture();
    _texture->setAntiAliasTexParameters();
    
    m_baseTex = _texture;
    
    this->initWithTexture(_texture);
    this->setFlipY(true);
    
    _renderTex->retain();

    float _anchorY = (_vline>=0.0)? (_height/2.0)/(_height + _vline) : (_height/2.0 + abs(_vline))/(_height + abs(_vline));   
    
    this->setAnchorPoint(Vec2(1.0f, _anchorY));
    this->setColor(Color3B(8,8,8));
    
    //this->setScale(0.5f);
}


////////
void staffSymbolLine::initSBLine(float _width, int _count)
{
    const int _linecount = _count>0?_count:1;
    
    const float _linewidth = 6.0;
    
    ////////
    RenderTexture* _renderTex = RenderTexture::create(
            _width, _linecount * _linewidth + _linewidth/2
            );
    
    DrawNode* drawnode = DrawNode::create();
    
    for( int i=1; i<= _linecount; i++ )
    {
        const float _height = _linecount * _linewidth + _linewidth / 2 - i*_linewidth;
        
        drawnode->drawSegment(Vec2(0,_height),Vec2(_width,_height),(i==1)?1.5:1.4, Color4F(0.0,0.0,0.0,1.0));    
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

    this->setAnchorPoint(Vec2(0.0,1.0));
    
}