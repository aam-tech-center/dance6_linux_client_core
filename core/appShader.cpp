/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   appShader.cpp
 * Author: Jack.L
 * 
 * Created on 2017年7月10日, 下午6:29
 */
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "appShader.h"
#include "coreUtil.h"

USING_NS_CC;

appShader::appShader() {
    
}

appShader::appShader(const appShader& orig) {
}

appShader::~appShader() {
}

void appShader::init()
{
    ////////
    std::string _path;
    
    addProgram(EPT_SINGLE_COLOR, "sampler.vsh", "sampler.fsh");
    addProgram(EPT_BW_COLOR, "sampler.vsh", "bwColor.fsh");
      

    
}

void appShader::addProgram(ENUM_PROGRAM_TYPE _type, const char* _vsh, const char* _fsh)
{
    ////////    
    Core::getCurrentPath();
    std::string _path = Core::getCurrentPath();
    _path += "/";
    
    
    std::string _fileVsh = _path;
    _fileVsh += "bin/shader/";
    _fileVsh += _vsh;
    
    std::string _fileFsh = _path;
    _fileFsh += "bin/shader/";    
    _fileFsh += _fsh;
    
    ////////    
    GLProgram* pGp =
    GLProgram::createWithFilenames(
            _fileVsh.c_str(),
            _fileFsh.c_str()           
    );
    
    pGp->link();
    
    m_programs[_type] = pGp;
    
}

GLProgram* appShader::getProgram(ENUM_PROGRAM_TYPE _type)
{
    return m_programs[_type];   
}















