//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_MATERIAL_H
#define OME_MATERIAL_H


#ifdef __cplusplus
extern "C" {
#endif


#include "color.h"


typedef struct omeTexture omeTexture;


// hold the representation of an OpenGL material
// Some components are stored in two forms: color and textures
typedef struct omeMaterial
{
    omeColor ambiantColor;
    omeColor diffuseColor;
    omeColor specularColor;
    omeColor emissiveColor;
    float shininess;

    // TODO: find a place for this one...?
    //omeTexture *ambiantTexture; // could be a cubemap texture?
    omeTexture *diffuseTexture;
    omeTexture *specularTexture;
    omeTexture *emissiveTexture;
    omeTexture *shininessTexture;
} omeMaterial;


// Constructor and destructor
omeMaterial *omeMaterialCreate(void);
void omeMaterialDestroy(omeMaterial **m);


#ifdef __cplusplus
}
#endif


#endif // OME_MATERIAL_H
