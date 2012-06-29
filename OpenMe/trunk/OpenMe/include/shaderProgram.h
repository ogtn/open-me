//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_PROGRAM_H
#define OME_PROGRAM_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include <uthash.h>


// forward declaration
typedef struct omeMaterial omeMaterial;


#define OME_PROGRAM_LOG_LENGTH          2048
// for uniforms and attributes names
// TODO: dynamic solution instead because of concatenation for structs, enums...
#define OME_PROGRAM_VAR_LENGTH          128


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
    unsigned int id;
    omeBool compiled;
} omeShader;


typedef enum omeProgramStatus
{
    OME_PROGRAM_STATUS_READY,
    OME_PROGRAM_STATUS_NOT_READY,
    OME_PROGRAM_STATUS_BROKEN,
    OME_PROGRAM_STATUS_MAX
} omeProgramStatus;


typedef enum omeLocationType
{
    OME_LOCATION_TYPE_UNIFORM,
    OME_LOCATION_TYPE_ATTRIB
} omeLocationType;


typedef struct omeLocation
{
    UT_hash_handle hh;
    char key[OME_PROGRAM_VAR_LENGTH];
    int location;
} omeLocation, omeLocationHashTable;


typedef struct omeProgram
{
    omeShader *shaders[OME_SHADER_TYPE_MAX];
    omeBool shadersAttached[OME_SHADER_TYPE_MAX];
    unsigned int id;
    omeBool linked;
    omeProgramStatus status;
    omeLocationHashTable *uniforms;
} omeProgram;


omeShader *omeShaderCreate(char *fileName);
void omeShaderDestroy(omeShader **s);
omeStatus omeShaderCompile(omeShader *s);

omeProgram *omeProgramCreate(void);
void omeProgramDestroy(omeProgram **sp);
void omeProgramAddShader(omeProgram *sp, omeShader *s);
omeStatus omeProgramPrepareLink(omeProgram *sp);
omeStatus omeProgramLink(omeProgram *sp);
void omeProgramUse(omeProgram *sp);
void omeProgramLocateUniforms(omeProgram *sp);
void omeProgramLocateAttributes(omeProgram *sp);
int omeProgramLocateUniform(omeProgram *sp, char *name);

void omeProgramSendUniformf(omeProgram *sp, float f, char *name);
void omeProgramSendUniformMaterial(omeProgram *p, omeMaterial *m, char *name);

omeLocation *omeLocationCreate(omeProgram *sp, char *name, omeLocationType type);
#define omeUniformLocationCreate(sp, name)  omeLocationCreate(sp, name, OME_LOCATION_TYPE_UNIFORM)
#define omeAttribLocationCreate(sp, name)   omeLocationCreate(sp, name, OME_LOCATION_TYPE_ATTRIB)
void omeLocationDestroy(omeLocation **loc);


#ifdef __cplusplus
}
#endif


#endif // OME_PROGRAM_H
