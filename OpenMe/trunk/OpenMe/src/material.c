//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "material.h"
#include <stdlib.h>
#include <string.h>


// default colors
static const omeColor defaultAmbiant  = {{0.1f, 0.1f, 0.1f, 1.f}};
static const omeColor defaultDiffuse  = {{0.3f, 0.3f, 0.3f, 1.f}};
static const omeColor defaultSpecular = {{0.9f, 0.9f, 0.9f, 1.f}};
static const omeColor defaultEmissive = {{0.0f, 0.0f, 0.0f, 1.f}};
static const float defaultShininess   = 64.0f;


omeMaterial *omeMaterialCreate(void)
{
    omeMaterial *m = calloc(1, sizeof(omeMaterial));

    m->ambiantColor = defaultAmbiant;
    m->diffuseColor = defaultDiffuse;
    m->specularColor = defaultSpecular;
    m->emissiveColor = defaultEmissive;
    m->shininess = defaultShininess;

    return m;
}


void omeMaterialDestroy(omeMaterial **m)
{
    // TODO: decrement texture ref here

    memset(*m, 0, sizeof(omeMaterial));
    free(*m);
    *m = NULL;
}
