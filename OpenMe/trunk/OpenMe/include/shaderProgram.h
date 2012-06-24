//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_SHADER_PROGRAM_H
#define OME_SHADER_PROGRAM_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include <GL/glew.h>


#define OME_SHADER_LOG_LENGTH       512


typedef enum omeShaderType
{
    OME_SHADER_TYPE_PIXEL,
    OME_SHADER_TYPE_VERTEX,
    OME_SHADER_TYPE_MAX
} omeShaderType;


typedef struct omeShader
{
    char *code;
    omeShaderType type;
    GLuint id;
    omeBool compiled;
} omeShader;


typedef enum omeShaderProgramStatus
{
    OME_SHADER_PROGRAM_STATUS_READY,
    OME_SHADER_PROGRAM_STATUS_NOT_READY,
    OME_SHADER_PROGRAM_STATUS_BROKEN,
    OME_SHADER_PROGRAM_STATUS_MAX
} omeShaderProgramStatus;


typedef struct omeShaderProgram
{
    omeShader *shaders[OME_SHADER_TYPE_MAX];
    omeBool shadersAttached[OME_SHADER_TYPE_MAX];
    GLuint id;
    omeBool linked;
    omeShaderProgramStatus status; 
} omeShaderProgram;


omeShader *omeShaderCreate(char *fileName);
void omeShaderDestroy(omeShader **s);
omeStatus omeShaderCompile(omeShader *s);

omeShaderProgram *omeShaderProgramCreate(void);
void omeShaderProgramDestroy(omeShaderProgram **sp);
void omeShaderProgramAddShader(omeShaderProgram *sp, omeShader *s);
omeStatus omeShaderProgramPrepareLink(omeShaderProgram *sp);
omeStatus omeShaderProgramLink(omeShaderProgram *sp);


#ifdef __cplusplus
}
#endif


#endif // OME_SHADER_PROGRAM_H
