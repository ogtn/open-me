//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_TEXTURE_H
#define OME_TEXTURE_H


#ifdef __cplusplus
extern "C" {
#endif


#include "resourceManager.h"


typedef enum omeTextureType
{
    OME_TEXTURE_TYPE_1D,
    OME_TEXTURE_TYPE_2D,
    OME_TEXTURE_TYPE_3D,
    OME_TEXTURE_TYPE_CUBE,
    OME_TEXTURE_TYPE_MAX
} omeTextureType;


typedef struct omeTexture
{
    // keep in first position
    omeResource resource;

    unsigned int type;
    int width;
    int height;
    int depth;
    unsigned int id;
} omeTexture;


omeTexture *omeTextureCreate(omeTextureType type, int width, int height, int depth, void **data, const char *name);
#define omeTextureCreate1D(width, data, name) omeTextureCreate(OME_TEXTURE_TYPE_1D, (width), 0, 0, (data), (name))
#define omeTextureCreate2D(width, height, data, name) omeTextureCreate(OME_TEXTURE_TYPE_2D, (width), (height), 0, (data), (name))
#define omeTextureCreate3D(width, heigt, depth, data, name) omeTextureCreate(OME_TEXTURE_TYPE_3D, (width), (height), (depth), (data), (name))
void omeTextureDestroy(omeTexture **t);
void omeTextureBind(omeTexture *t);
// TODO: should be private?
unsigned int omeTextureGetGLType(omeTextureType type);

omeTexture *omeTextureLoadFromFile(const char *fileName);


#ifdef __cplusplus
}
#endif


#endif // OME_TEXTURE_H
