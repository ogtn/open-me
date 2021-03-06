﻿//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_MESH_H
#define OME_MESH_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include "entity.h"


// forward declaration
typedef struct omeGeometry omeGeometry;
typedef enum omePolygonType omePolygonType;
typedef struct omeProgram omeProgram;
typedef struct omeMaterial omeMaterial;


typedef struct omeMesh
{
    // keep first
    omeEntity entity;

    int nbBuffers;
    int geometryCpt;
    int nbFinalizedBuffers;
    omeGeometry **geometries;
    omeBool finalized;  //TODO: is this still usefull to keep that one? If it is, use this variable somewhere...
    omeProgram *program;
    omeMaterial *material;
} omeMesh;


// TODO: use a macro for generating the typedef and the struct OME_TYPEDEF_LIST(type) OME_LIST(type)?
typedef struct omeMeshListElement omeMeshListElement, omeMeshList;
struct omeMeshListElement
{
    omeMesh *mesh;
    omeMeshListElement *next;
    omeMeshListElement *prev;
};


omeMesh *omeMeshCreate(int nbBuffers);
void omeMeshDestroy(omeMesh **m);
omeGeometry *omeMeshAddGeometry(omeMesh *m, omeGeometry *g);
void omeMeshFinalize(omeMesh *m);
void omeMeshBufferFinalized(omeMesh *m);
void omeMeshSave(const char *fileName, omeMesh *m);
omeMesh *omeMeshLoad(const char *fileName);

omeMesh *omePrimitiveCube(float size, int subdivisions);
omeMesh *omePrimitiveSphere(float radius, int subdivisions);
omeMesh *omePrimitiveGrid(int size, int level);


#ifdef __cplusplus
}
#endif


#endif // OME_MESH_H
