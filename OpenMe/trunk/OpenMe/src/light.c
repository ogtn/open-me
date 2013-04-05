//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "light.h"
#include <stdio.h>
#include <stdlib.h>


static const omeColor3 defaultAmbiant =  {{0.1f, 0.1f, 0.1f}};
static const omeColor3 defaultDiffuse =  {{0.3f, 0.3f, 0.3f}};
static const omeColor3 defaultSpecular = {{0.9f, 0.9f, 0.9f}};


omeLight *omeLightCreate(omeLightType type)
{
    static int lightCpt = 0;
    char name[OME_NAME_MAXLEN];
    omeLight *l = calloc(1, sizeof(omeLight));

    sprintf(name, "light_%d", lightCpt++);
    omeEntityInit(l, OME_ENTITY_OMNI_LIGHT, name);
    l->type = type;
    l->ambiantColor = defaultAmbiant;
    l->diffuseColor = defaultDiffuse;
    l->specularColor = defaultSpecular;

    return l;
}


void omeLightDestroy(omeLight **l)
{
    omeDbgClearMem(*l, sizeof(omeLight));

    free(*l);
    *l = NULL;
}
