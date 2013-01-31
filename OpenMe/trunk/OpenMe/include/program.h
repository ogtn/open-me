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
#include "shader.h"
#include "uthash.h"


// forward declaration
typedef struct omeMaterial omeMaterial;
typedef struct omeCamera omeCamera;
typedef union omeVector omeVector;
typedef union omeMatrix omeMatrix;
typedef struct omeLight omeLight;


// for uniforms and attributes names
// TODO: dynamic solution instead because of concatenation for structs, enums...
#define OME_PROGRAM_VAR_LENGTH          128


typedef enum omeProgramStatus
{
    OME_PROGRAM_STATUS_READY,
    OME_PROGRAM_STATUS_NOT_READY,
    OME_PROGRAM_STATUS_BROKEN
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
    omeLocationHashTable *attributes;
} omeProgram;


omeProgram *omeProgramCreate(void);
void omeProgramDestroy(omeProgram **sp);
void omeProgramAddShader(omeProgram *sp, omeShader *s);
omeStatus omeProgramPrepareLink(omeProgram *sp);
omeStatus omeProgramLink(omeProgram *sp);
void omeProgramUse(omeProgram *sp);
void omeProgramLocateUniforms(omeProgram *sp);
void omeProgramLocateAttributes(omeProgram *sp);
int omeProgramLocateUniform(omeProgram *sp, const char *name);
int omeProgramLocateAttribute(omeProgram *sp, const char *name);

void omeProgramSendUniformf(omeProgram *sp, float f, const char *name);
void omeProgramSendUniformVec(omeProgram *sp, const omeVector *v, const char *name);
void omeProgramSendUniformMaterial(omeProgram *p, omeMaterial *m, const char *name);
void omeProgramSendUniformEntity(omeProgram *p, void *object);
void omeProgramSendUniformCamera(omeProgram *p, omeCamera *c);
void omeProgramSendUniformMatrix(omeProgram *p, omeMatrix *m, const char *name);
void omeProgramSendUniformLight(omeProgram *p, omeLight *l, const char *name);

omeLocation *omeLocationCreate(omeProgram *sp, const char *name, omeLocationType type);
#define omeUniformLocationCreate(sp, name)  omeLocationCreate(sp, name, OME_LOCATION_TYPE_UNIFORM)
#define omeAttribLocationCreate(sp, name)   omeLocationCreate(sp, name, OME_LOCATION_TYPE_ATTRIB)
void omeLocationDestroy(omeLocation **loc);


#ifdef __cplusplus
}
#endif


#endif // OME_PROGRAM_H
