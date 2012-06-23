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
typedef struct omeBuffer omeBuffer;
typedef enum omePolygonType omePolygonType;


// TODO: only VBO in GL >= 3.x, get rid of that
typedef enum omeRenderType
{
    OME_IMMEDIATE,
    OME_VERTEX_ARRAY,
    OME_VERTEX_BUFFER_OBJECT
} omeRenderType;


typedef struct omeMesh
{
    int nbBuffers;
    int bufferCpt;
    int nbFinalizedBuffers;
    omeBuffer **buffers;
    omeBool finalized;
    omeRenderType  renderType;
    //omeEntity entity;
} omeMesh;


omeMesh *omeMeshCreate(int nbBuffers);
void omeMeshDestroy(omeMesh **m);
omeBuffer *omeMeshAddBuffer(omeMesh *m, int nbVertices, int nbAttributes, omePolygonType polygonType);
void omeMeshFinalize(omeMesh *m);
void omeMeshBufferFinalized(omeMesh *m);
void omeMeshRender(omeMesh *m);
void omeMeshRenderImmediate(omeMesh *m);
void omeMeshRenderVA(omeMesh *m);
void omeMeshRenderVBO(omeMesh *m);
void omeMeshSave(char *filename, omeMesh *m);
omeMesh *omeMeshLoad(char *filename);


#ifdef __cplusplus
}
#endif


#endif // OME_MESH_H
