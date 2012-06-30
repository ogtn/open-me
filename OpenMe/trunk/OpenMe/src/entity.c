//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "entity.h"
#include "logger.h"
#include <string.h>
#include <stdlib.h>


static const omeVector defaultPosition =	{{0, 0, 0}};
static const omeVector defaultScaling = 	{{1, 1, 1}};
static const omeVector defaultRotation = 	{{0, 0, 0}};


void omeEntityInit(void *object, omeEntityType type, const char *name)
{
    omeEntity *e = object;

    e->magic = OME_ENTITY_MAGIC;
    e->type = type;

    omeVectorCopy(&e->position, &defaultPosition);
    omeVectorCopy(&e->scaling, &defaultScaling);
    omeVectorCopy(&e->rotation, &defaultRotation);
    strncpy(e->name, name, OME_NAME_MAXLEN);
}


void omeEntitySetPosition(void *object, const omeVector *pos)
{
    omeEntity *e = object;
    OME_ENTITY_CHECK(e);

	e->position = *pos;
}


void omeEntityMove(void *object, const omeVector *displacement)
{
    omeEntity *e = object;
    OME_ENTITY_CHECK(e);

	omeVectorAddVector(&e->position, displacement, &e->position);
}
