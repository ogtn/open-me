//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_RESOURCE_MANAGER_H
#define OME_RESOURCE_MANAGER_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include <uthash.h>


#define OME_RESOURCE_MAGIC			0x42042042


typedef enum omeResourceType
{
	OME_RESOURCE_TEXTURE,
	OME_RESOURCE_PIXEL_SHADER,
	OME_RESOURCE_VERTEX_SHADER,
	OME_RESOURCE_MATERIAL,
	OME_RESOURCE_GEOMETRY,
	OME_RESOURCE_MAX
} omeResourceType;


typedef struct omeResource
{
	unsigned int magic;
	omeResourceType type;
    UT_hash_handle *hashHandle;
    char hashKey[OME_NAME_MAXLEN];
} omeResource;


typedef struct omeResourceLoader
{
    UT_hash_handle *hashHandle;
    char hashKey[OME_NAME_MAXLEN];
    //loadFunction;
    //saveFunction;
} omeResourceLoader;


typedef struct omeResourceManager
{
    omeResource *resources;
} omeResourceManager;


#ifdef __cplusplus
}
#endif


#endif // OME_RESOURCE_MANAGER_H
