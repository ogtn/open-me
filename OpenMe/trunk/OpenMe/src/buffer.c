//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "buffer.h"
#include "logger.h"
#include "mesh.h"
#include <stdlib.h>
#include <string.h>


omeBuffer *omeBufferCreate(int nbVertices, int nbAttributes, omeMesh *m)
{
    omeBuffer *b = calloc(1, sizeof(omeBuffer));

    // default values TODO: add setters to modifiy them or even better -> self adapted solution based on a benchmark :D
    b->indexed = OME_FALSE;
    b->interleaved = OME_FALSE;
    b->padded = OME_FALSE;

    b->nbVertices = nbVertices;
    b->nbAttributes = nbAttributes;
    b->mesh = m;

    return b;
}


void omeBufferDestroy(omeBuffer **b)
{
    int i = 0;

    //TODO: figure out what else do we need to free here...

    memset(*b, 0, sizeof(omeBuffer));
    free(*b);
    *b = NULL;
}


int omeBufferAddAttrib(omeBuffer *b, int nbElements, omeType type, int updateHint, omeBufferType bufferType, void *data)
{
    omeVertexAttrib *attrib;
    
    if(b->indexCpt >= b->nbAttributes)
    {
        omeLoggerLog("All attributes have already been added");
        return -1;
    }

    // TODO: play with indexes to automatize that, or add a special function for position??? (check opengl 4.2 documentation)
    if((b->indexCpt == 0 && bufferType != OME_BUFFER_TYPE_POSITION) ||
        (bufferType == OME_BUFFER_TYPE_POSITION && b->indexCpt != 0))
    {
        omeLoggerLog("vertex position must be added first");
        return -1;
    }

    attrib = &b->attributes[b->indexCpt];
    attrib->nbElements = nbElements;
    attrib->type = type;
    attrib->updateHint = updateHint;
    attrib->bufferType = bufferType;
    attrib->data = data;
    attrib->size = b->nbVertices * nbElements * omeSizeOf(type);

    b->vertexSize += omeSizeOf(type);
    b->indexCpt++;

    if(b->indexCpt == b->nbAttributes)
        omeBufferFinalize(b);

    return b->indexCpt;
}


void omeBufferFinalize(omeBuffer *b)
{
    int i;
    char *ptr;

    // TODO: remove thoses tests once the function will be private?
    if(b->indexCpt != b->nbAttributes)
    {
        omeLoggerLog("Can't finalize, attributes are missing");
        return;
    }
    
    if(b->finalized)
    {
        omeLoggerLog("Allready finalized");
        return;
    }

    //TODO: take the hints and the options into account somewhere around here

    if(b->mesh != NULL)
        omeMeshBufferFinalized(b->mesh);

     b->finalized = OME_TRUE;
}


void omeBufferUpdateAttrib(omeBuffer *b, int attribIndex, void *data)
{
    //TODO: implement that :)
}
