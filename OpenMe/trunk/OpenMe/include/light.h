//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_LIGHT_H
#define OME_LIGHT_H


#ifdef __cplusplus
extern "C" {
#endif


#include "entity.h"
#include "color.h"


typedef enum omeLightType
{
    OME_LIGTH_TYPE_OMNI,	// point light which emits in all directions
    OME_LIGHT_TYPE_MAX
} omeLightType;


// TODO: does it even make sense to have 3 types of colors for one light? => KISS
typedef struct omeLight
{
    // keep in first place
    omeEntity entity;

    omeLightType type;
    omeColor3 ambiantColor;
    omeColor3 diffuseColor;
    omeColor3 specularColor;
} omeLight;



omeLight *omeLightCreate(omeLightType type);
#define omeLightCreateOmni() omeLightCreate(OME_LIGTH_TYPE_OMNI)
void omeLightDestroy(omeLight **l);


#ifdef __cplusplus
}
#endif


#endif // OME_LIGHT_H
