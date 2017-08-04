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
    for( int i=0; i<MAX_TEXTURE_COUNT; i++ )
    {
        m_tex[i] = NULL;
    }
}

renderSprite::~renderSprite() {
}

void renderSprite::setTexture(int _index, Texture2D* _tex)
{
    if( _tex != NULL && _index >= 0 && _index < MAX_TEXTURE_COUNT )
    {
        m_tex[_index] = _tex;
    }
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
    appShader::Instance()->getProgram(EPT_TEXSCOMPLEX);
    
    _pgp->link();
    _pgp->updateUniforms();
        
    this->setGLProgram(_pgp);
    
    this->retain();
    
    /////////
    char _strTemp[64];
    for( int i=0; i<MAX_TEXTURE_COUNT; i++ )
    {        
        memset(_strTemp, 0, sizeof(_strTemp));        
        sprintf(
                _strTemp,
                //"CC_Texture%d", 
                "texture%d",
                i
                );
        
        m_texHandle[i] = glGetUniformLocation(_pgp->getProgram(), _strTemp);
    }
    
    return true;
}

void renderSprite::draw(Renderer* render, const Mat4& transform, uint32_t transformUpdated)
{
    m_customCommand.init(_globalZOrder);
    m_customCommand.func = CC_CALLBACK_0(renderSprite::onDraw, this);
    render->addCommand(&m_customCommand);
    
    Sprite::draw( render, transform, transformUpdated );
}

void renderSprite::onDraw()
{
    /*
    glEnable(GL_DEPTH_TEST);
    CC_NODE_DRAW_SETUP();    
    CHECK_GL_ERROR_DEBUG();
    
    for( int i=0; i<MAX_TEXTURE_COUNT; i++ )
    {
        if( m_tex[i] != NULL )
        {
            GL::activeTexture(GL_TEXTURE0 + i);
            GL::bindTexture2DN(i, m_tex[i]->getName());
            glUniform1i(m_texHandle[i], i);
        }
    }
    
    CHECK_GL_ERROR_DEBUG();
            
    ////////
    glDisable(GL_DEPTH_TEST);
    CHECK_GL_ERROR_DEBUG();
    */
    
    /*
    GL::bindTexture2DN(0, m_tex[0]->getName());
    GL::bindTexture2DN(1, m_tex[1]->getName());
    CHECK_GL_ERROR_DEBUG();
    */
}