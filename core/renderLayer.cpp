/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   renderLayer.cpp
 * Author: Jack.L
 * 
 * Created on 2017年8月1日, 上午11:32
 */

#include "renderLayer.h"

USING_NS_CC;
    
////////
renderLayer::renderLayer():
Layer::Layer()
{  
    ////////    
    m_texHandle[0] = 0;
    m_texHandle[1] = 0;
                
    m_texID[0] = -1;
    m_texID[1] = -1;
    
    
}

renderLayer::~renderLayer()
{    
}

void renderLayer::setLayerGLProgram(cocos2d::GLProgram* _pgp)
{    
    m_texHandle[0] = glGetUniformLocation( _pgp->getProgram(), "texture0");
    m_texHandle[1] = glGetUniformLocation( _pgp->getProgram(), "texture1");
    
    m_program = _pgp;    
    
    this->setGLProgram(m_program);
}

bool renderLayer::init()
{
    bool success = Layer::init();

    if( success )
    {
        
    }
    
    return success;
}

void renderLayer::draw(cocos2d::Renderer* render, const cocos2d::Mat4& transform, uint32_t transformUpdated)
{
    Layer::draw(render, transform, transformUpdated);
    
    m_customCommand.init(_globalZOrder);
    m_customCommand.func = CC_CALLBACK_0(renderLayer::onDraw, this);
    render->addCommand( &m_customCommand );
    
}

void renderLayer::onDraw()
{    
    CHECK_GL_ERROR_DEBUG();

    //Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    //Director::getInstance()->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    
    //Mat4 _mat = Mat4::ZERO;//Mat4::IDENTITY;
    
    
    //Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _mat);
    
    Director::getInstance()->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    
    Mat4  _mat = Mat4::IDENTITY;
    //_mat.rotateZ(PI);
    _mat.scale(0.5f);
    
    Director::getInstance()->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _mat);
    
    //Director::getInstance()->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

    auto glProgram = m_program;
    glProgram->use();
    glProgram->setUniformsForBuiltins();
    CHECK_GL_ERROR_DEBUG();  
            
    ////////
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);      
    glEnable(GL_BLEND);  
    
    glEnable(GL_DEPTH_TEST);
    CHECK_GL_ERROR_DEBUG();  
    
    ////////    
    CHECK_GL_ERROR_DEBUG();
        
    if( m_texID[0] >= 0 && m_texID[0] < 0xFF )
    {
        glUniform1i(m_texHandle[0], 0);
        GL::bindTexture2DN(0, m_texID[0]);
        
        CHECK_GL_ERROR_DEBUG();
    }
    
    if( m_texID[1] >= 0 && m_texID[1] < 0xFF )
    {        
        glUniform1i(m_texHandle[1], 1);
        GL::bindTexture2DN(1, m_texID[1]);
        
        CHECK_GL_ERROR_DEBUG();
    }        
    
    ////////    
    CHECK_GL_ERROR_DEBUG();
    
    ////////
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, 4);

    ////////
    CHECK_GL_ERROR_DEBUG();
    glDisable(GL_DEPTH_TEST);
 
    ////////
    Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    //Director::getInstance()->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        
}

void renderLayer::setTextureID(GLuint _texid, int index)
{
    m_texID[index] = _texid;
}