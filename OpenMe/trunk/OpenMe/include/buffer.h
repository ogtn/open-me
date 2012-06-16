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


//TODO: should be private. Internal use only
typedef struct omeVertexAttrib
{
    int nbElement;
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
    omeVertexAttrib attributes[OME_MAX_ATTRIB];  //TODO: try dynamic solution instead?
    char *data[OME_MAX_ATTRIB];                  //TODO: try dynamic solution instead?
    omeBool interleaved;
    omeBool indexed;
    omeBool padded;
    omeBool finalized;
} omeBuffer;


omeBuffer *omeBufferCreate(int nbVertices, int nbAttributes);
void omeBufferDestroy(omeBuffer **b);
int omeBufferAddAttrib(omeBuffer *b, int nbElement, omeType type, int updateHint, omeBufferType bufferType, void *data);
void omeBufferFinalize(omeBuffer *b);
void omeBufferUpdateAttrib(omeBuffer *b, int attribIndex, void *data);


#endif // OME_BUFFER_H
