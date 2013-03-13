//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_ENTITY_H
#define OME_ENTITY_H


#ifdef __cplusplus
extern "C" {
#endif


#include "vector.h"
#include "utils.h"


#define OME_ENTITY_MAGIC        0x66666666

// TODO: disable this in release?
#define OME_ENTITY_CHECK(ptr)   do\
    {\
    if(((omeEntity*)(ptr))->magic != OME_ENTITY_MAGIC)\
            omeLoggerLog("Polymorphism panic: dynamic cast failed!\n");\
            return;\
    } while(0);


typedef enum omeEntityType
{
    OME_ENTITY_MESH,
    OME_ENTITY_OMNI_LIGHT,
    OME_ENTITY_CAMERA,
    OME_ENTITY_MAX
} omeEntityType;


typedef struct omeEntity
{
    // header
    unsigned int magic;
    omeEntityType type;

    omeVector position;
    omeVector scaling;
    omeVector rotation;
    //TODO: very basic string module?
    char name[OME_NAME_MAXLEN];
} omeEntity;


void omeEntityInit(void *object, omeEntityType type, const char *name);
void omeEntitySetPosition(void *object, const omeVector *pos);
void omeEntityMove(void *object, const omeVector *displacement);


#ifdef __cplusplus
}
#endif


#endif // OME_ENTITY_H
