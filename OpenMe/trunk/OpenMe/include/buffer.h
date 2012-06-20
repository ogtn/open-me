//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_BUFFER_H
#define OME_BUFFER_H


#include "utils.h"
#include <GL/glew.h>


// forward declaration
typedef struct omeMesh omeMesh;


typedef enum omeBufferType
{
    OME_BUFFER_TYPE_POSITION,
    OME_BUFFER_TYPE_TEXCOORD_0,
    OME_BUFFER_TYPE_TEXCOORD_1,
    OME_BUFFER_TYPE_TEXCOORD_2,
    OME_BUFFER_TYPE_TEXCOORD_3,
    OME_BUFFER_TYPE_NORMAL,
    OME_BUFFER_TYPE_COLOR,
    OME_BUFFER_TYPE_INDEX,
    OME_BUFFER_TYPE_USER_0,
    OME_BUFFER_TYPE_USER_1,
    OME_BUFFER_TYPE_USER_2,
    OME_BUFFER_TYPE_USER_3,
    OME_BUFFER_TYPE_MAX
} omeBufferType;


typedef enum omePolygonType
{
    OME_POINTS,
    OME_LINE_STRIP,
    OME_LINE_LOOP,
    OME_LINES,
    OME_LINE_STRIP_ADJACENCY,
    OME_LINES_ADJACENCY,
    OME_TRIANGLE_STRIP,
    OME_TRIANGLE_FAN,
    OME_TRIANGLES,
    OME_TRIANGLE_STRIP_ADJACENCY,
    OME_TRIANGLES_ADJACENCY,
    OME_PATCHES
} omePolygonType;


//TODO: should be private. Internal use only
typedef struct omeVertexAttrib
{
    int nbElements;
    int size;
    char *data;
    int updateHint;
    omeType type;
    omeBufferType bufferType;
} omeVertexAttrib;


#define OME_MAX_ATTRIB  8


typedef struct omeBuffer
{
    int nbVertices;
    int nbAttributes;
    int vertexSize;
    int indexCpt;
    int size;
    omeVertexAttrib attributes[OME_MAX_ATTRIB];  //TODO: try dynamic solution instead?
    omeVertexAttrib indices;
    omeBool interleaved;
    omeBool indexed;
    omeBool padded;
    omeBool finalized;
    omeMesh *mesh;
    omePolygonType polygonType;
    GLuint VBO;
    omeBool VBOReady;
} omeBuffer;


omeBuffer *omeBufferCreate(int nbVertices, int nbAttributes, omePolygonType polygonType, omeMesh *m);
void omeBufferDestroy(omeBuffer **b);
int omeBufferAddAttrib(omeBuffer *b, int nbElements, omeType type, int updateHint, omeBufferType bufferType, void *data);
int omeBufferAddIndices(omeBuffer *b, omeType type, int updateHint, void *data);
void omeBufferFinalize(omeBuffer *b);
void omeBufferUpdateAttrib(omeBuffer *b, int attribIndex, void *data);
void omeBufferRenderVA(omeBuffer *b);
void omeBufferRenderVBO(omeBuffer *b);
void omeBufferUseIndices(omeBuffer *b);


#endif // OME_BUFFER_H
