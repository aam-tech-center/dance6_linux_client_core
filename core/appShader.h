/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   appShader.h
 * Author: Jack.L
 *
 * Created on 2017年7月10日, 下午6:29
 */

#ifndef APPSHADER_H
#define APPSHADER_H

#include "cocos2d.h"

enum ENUM_PROGRAM_TYPE
{
    EPT_SINGLE_COLOR = 0,
    EPT_BW_COLOR,
    
    EPT_COUNT,
};

class appShader 
{
protected:
    
    cocos2d::GLProgram* m_programs[EPT_COUNT];
    
    appShader();
    appShader(const appShader& orig);
    virtual ~appShader();
    
    void addProgram(ENUM_PROGRAM_TYPE _type, const char* _vsh, const char* _fsh);
    
public:

    void init();

    static appShader* Instance()
    {
        static appShader _shader;
        return &_shader;
    }
    
    cocos2d::GLProgram* getProgram(ENUM_PROGRAM_TYPE _type);
};

#endif /* APPSHADER_H */

