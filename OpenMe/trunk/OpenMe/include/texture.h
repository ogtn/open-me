//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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
    OME_TEXTURE_TYPE_CUBE
} omeTextureType;


#include "private_access_checking.h"

// Encapsulate an OpenGL texture
// Data only stored in OpenGL context for the moment
typedef struct omeTexture
{
    // keep in first position
    omeResource resource;

    private unsigned int type;  // OpenGL type of texture
    private int width;          // texture width
    private int height;         // texture height 
    private int depth;          // texture depth
    private unsigned int id;
} omeTexture;

#include "private_access_checking.h"


// generic function to create a texture
omeTexture *omeTextureCreate(omeTextureType type, int width, int height, int depth, void **data, const char *name);

// alias used to simplifie texture creation
// arguments have the same meaning that in omeTextureCreate()
#define omeTextureCreate1D(width, data, name) omeTextureCreate(OME_TEXTURE_TYPE_1D, (width), 0, 0, (data), (name))
#define omeTextureCreate2D(width, height, data, name) omeTextureCreate(OME_TEXTURE_TYPE_2D, (width), (height), 0, (data), (name))
#define omeTextureCreate3D(width, heigt, depth, data, name) omeTextureCreate(OME_TEXTURE_TYPE_3D, (width), (height), (depth), (data), (name))
#define omeTextureCreateCubeMap(width, height, data, name) omeTextureCreate(OME_TEXTURE_TYPE_CUBE, (width), (height), 0, (data), (name))

// 
void omeTextureDestroy(omeTexture **t);
void omeTextureBind(omeTexture *t);
// TODO: should be private?
unsigned int omeTextureGetGLType(omeTextureType type);

omeTexture *omeTextureLoadFromFile(const char *fileName);
omeTexture *omeTextureCubeMapLoadFromFile(const char *fileName);


#ifdef __cplusplus
}
#endif


#endif // OME_TEXTURE_H
