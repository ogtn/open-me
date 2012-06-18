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


#include "vector.h"
#include "utils.h"
#include <wchar.h>


typedef struct omeEntity
{
    omeVector position;
    omeVector scaling;
    omeVector rotation;
    //TODO: very basic string module?
    wchar_t name[OME_NAME_MAXLEN];
} omeEntity;


omeEntity *omeEntityCreate(wchar_t *name);
void omeEntityDestroy(omeEntity **e);


#endif // OME_ENTITY_H