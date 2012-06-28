//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
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


// forward declaration
typedef struct omeMatrix omeMatrix;


#include "vector.h"
#include "utils.h"
#include <wchar.h>


typedef struct omeEntity
{
	omeMatrix *modelview;
    omeVector position;
    omeVector scaling;
    omeVector rotation;
    //TODO: very basic string module?
    char name[OME_NAME_MAXLEN];
} omeEntity;


omeEntity *omeEntityCreate(wchar_t *name);
void omeEntityDestroy(omeEntity **e);


#ifdef __cplusplus
}
#endif


#endif // OME_ENTITY_H
