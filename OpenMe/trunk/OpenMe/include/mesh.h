//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
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


// forward declaration
typedef struct omeGeometry omeGeometry;
typedef enum omePolygonType omePolygonType;
typedef struct omeProgram omeProgram;
typedef struct omeMaterial omeMaterial;


// TODO: only VBO in GL >= 3.x, get rid of that
typedef enum omeRenderType
{
    OME_IMMEDIATE,
    OME_VERTEX_ARRAY,
    OME_VERTEX_GEOMETRY_OBJECT
} omeRenderType;


typedef struct omeMesh
{
    int nbBuffers;
    int geometryCpt;
    int nbFinalizedBuffers;
    omeGeometry **geometries;
    omeBool finalized;
    omeRenderType renderType;
    omeProgram *program;
    omeMaterial *material;
    //omeEntity entity;
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
void omeMeshRender(omeMesh *m);
void omeMeshRenderImmediate(omeMesh *m);
void omeMeshRenderVA(omeMesh *m);
void omeMeshRenderVBO(omeMesh *m);
void omeMeshSave(char *fileName, omeMesh *m);
omeMesh *omeMeshLoad(char *fileName);


#ifdef __cplusplus
}
#endif


#endif // OME_MESH_H
