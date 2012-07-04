//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

#ifndef OME_SHADER_H
#define OME_SHADER_H


#ifdef __cplusplus
extern "C" {
#endif


#include "resourceManager.h"


typedef enum omeShaderType
{
    OME_SHADER_TYPE_PIXEL,
    OME_SHADER_TYPE_VERTEX,
    OME_SHADER_TYPE_MAX
} omeShaderType;


typedef struct omeShader
{
    // keep in first position
    omeResource resource;

    char *code;
    omeShaderType type;
    unsigned int id;
    omeBool compiled;
} omeShader;


omeShader *omeShaderCreate(omeShaderType type, char *code, const char *name);
omeShader *omeShaderLoadFromFile(const char *fileName);
void omeShaderDestroy(omeShader **s);
omeStatus omeShaderCompile(omeShader *s);


#ifdef __cplusplus
}
#endif


#endif // OME_SHADER_H
