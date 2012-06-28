//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "entity.h"
#include <string.h>
#include <wchar.h>
#include <stdlib.h>


static const omeVector defaultPosition =	{{0, 0, 0}};
static const omeVector defaultScaling = 	{{1, 1, 1}};
static const omeVector defaultRotation = 	{{0, 0, 0}};


omeEntity *omeEntityCreate(wchar_t *name)
{
    omeEntity *e = calloc(1, sizeof(omeEntity));

    omeVectorCopy(&e->position, &defaultPosition);
    omeVectorCopy(&e->scaling, &defaultScaling);
    omeVectorCopy(&e->rotation, &defaultRotation);
    wcsncpy(e->name, name, OME_NAME_MAXLEN);

    return e;
}


void omeEntityDestroy(omeEntity **e)
{
    memset(*e, 0, sizeof(omeEntity));
    free(*e);
    *e = NULL;
}


void omeEntitySetPosition(omeEntity *e, omeVector *pos)
{
	e->position = *pos;
}


void omeEntityMove(omeEntity *e, omeVector *displacement)
{
	omeVectorAddVector(&e->position, displacement, &e->position);
}
