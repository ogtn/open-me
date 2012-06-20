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
#include <GL/glew.h>


GLenum omePolygonTypeToGL(omePolygonType polygonType)
{
    switch(polygonType)
    {
    case OME_POINTS​: return GL_POINTS;
    case OME_LINE_STRIP​: return GL_LINE_STRIP;
    case OME_LINE_LOOP​: return GL_LINE_LOOP;
    case OME_LINES​: return GL_LINES;
    case OME_LINE_STRIP_ADJACENCY​: return GL_LINE_STRIP_ADJACENCY;
    case OME_LINES_ADJACENCY​: return GL_LINES_ADJACENCY;
    case OME_TRIANGLE_STRIP​: return GL_TRIANGLE_STRIP;
    case OME_TRIANGLE_FAN​: return GL_TRIANGLE_FAN;
    case OME_TRIANGLES​: return GL_TRIANGLES;
    case OME_TRIANGLE_STRIP_ADJACENCY​: return GL_TRIANGLE_STRIP_ADJACENCY;
    case OME_TRIANGLES_ADJACENCY​: return GL_TRIANGLES_ADJACENCY;
    case OME_PATCHES: return GL_PATCHES;
    default: omeLoggerLog("Invalid polygon type");
    }

    return GL_POINTS;
}


omeBuffer *omeBufferCreate(int nbVertices, int nbAttributes, omePolygonType polygonType, omeMesh *m)
{
    omeBuffer *b = calloc(1, sizeof(omeBuffer));

    // default values TODO: add setters to modifiy them or even better -> self adapted solution based on a benchmark :D
    b->indexed = OME_FALSE;
    b->interleaved = OME_FALSE;
    b->padded = OME_FALSE;

    b->nbVertices = nbVertices;
    b->nbAttributes = nbAttributes;
    b->polygonType = polygonType;
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

    // TODO: play with indices to automatize that, or add a special function for position??? (check opengl 4.2 documentation)
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


int omeBufferAddIndices(omeBuffer *b, omeType type, int updateHint, void *data)
{
    omeVertexAttrib *attrib;

    if(b->indexed == OME_FALSE)
    {
        omeLoggerLog("This mesh is not indexed");
        return -1;
    }

    attrib = &b->indices;
    attrib->nbElements = 1;
    attrib->type = type;
    attrib->updateHint = updateHint;
    attrib->bufferType = OME_BUFFER_TYPE_INDEX;
    attrib->data = data;
    attrib->size = b->nbVertices * omeSizeOf(type);

    return b->nbAttributes;
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


void omeBufferRenderVA(omeBuffer *b)
{
     /*
    GL_COLOR_ARRAY
    GL_EDGE_FLAG_ARRAY
    GL_FOG_COORD_ARRAY
    GL_INDEX_ARRAY
    GL_NORMAL_ARRAY
    GL_SECONDARY_COLOR_ARRAY
    GL_TEXTURE_COORD_ARRAY
    GL_VERTEX_ARRAY
    */

    int i;
    omeVertexAttrib *attr = NULL;

    for(i = 0; i < b->nbAttributes; i++)
    {
        attr = &b->attributes[i];

        if(attr->bufferType == OME_BUFFER_TYPE_POSITION)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else if(attr->bufferType == OME_BUFFER_TYPE_COLOR)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else
        {
            omeLoggerLog("Not implemented yet\n");
            return; //TODO: find a way to not skip the rest (goto??)
        }
    }

    if(b->indexed)
    {
        if(b->indices.type != OME_UBYTE)
        {
            omeLoggerLog("Not implemented yet\n");
            return;
        }

        glDrawElements(omePolygonTypeToGL(b->polygonType), b->nbVertices, GL_UNSIGNED_BYTE, b->indices.data);
    }
    else
        glDrawArrays(omePolygonTypeToGL(b->polygonType), 0, b->nbVertices);

    for(i = 0; i < b->nbAttributes; i++)
    {
        attr = &b->attributes[i];

        if(attr->bufferType == OME_BUFFER_TYPE_POSITION)
            glDisableClientState(GL_VERTEX_ARRAY);
        else if(attr->bufferType == OME_BUFFER_TYPE_COLOR)
            glDisableClientState(GL_COLOR_ARRAY);
    }
}

/*
void omeBufferRenderVBO(omeBuffer *b)
{
    GLenum arrayType = GL_VERTEX_ARRAY;
    int i;

    if(b->VBOReady == OME_FALSE)
    {
        int vertexSize = 0;

        glGenBuffers(1, &b->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, b->VBO);

        //TODO: use hints here for the last parameter
        glBufferData(GL_ARRAY_BUFFER, b->vertexSize * b->nbVertices, NULL, GL_STATIC_DRAW);

         for(i = 0; i < b->nbAttributes; i++)
         {
             omeVertexAttrib *attr = &b->attributes[i];

             glBufferSubData(GL_ARRAY_BUFFER, vertexSize * b->nbVertices, b->nbVertices * attr->size * omeSizeOf(attr->type), attr->data);
             vertexSize += b->nbVertices * omeSizeOf(attr->type);
         }

         b->VBOReady = OME_TRUE;
    }

    glBindBuffer(GL_ARRAY_BUFFER, b->VBO);

    for(i = b->nbAttributes - 1; i >= 0; i--)
    {
        omeVertexAttrib *attr = &b->attributes[i];

        if(attr->bufferType == OME_BUFFER_TYPE_POSITION)
        {
            arrayType = GL_VERTEX_ARRAY;
            glEnableClientState(arrayType);
            glVertexPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else if(attr->bufferType == OME_BUFFER_TYPE_COLOR)
        {
            arrayType = GL_COLOR_ARRAY;
            glEnableClientState(arrayType);
            glColorPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else
        {
            omeLoggerLog("Not implemented yet\n");
            return; //TODO: find a way to not skip the rest (goto??)
        }
    }

    if(b->indexed)
    {
        if(b->indices.type != OME_UBYTE)
        {
            omeLoggerLog("Not implemented yet\n");
            return;
        }

        glDrawElements(omePolygonTypeToGL(b->polygonType), b->nbVertices, GL_UNSIGNED_BYTE, b->indices.data);
    }
    else
        glDrawArrays(omePolygonTypeToGL(b->polygonType), 0, b->nbVertices);

    for(i = b->nbAttributes - 1; i >= 0; i--)
        glDisableClientState(arrayType);
}
*/

void omeBufferUseIndices(omeBuffer *b)
{
    b->indexed = OME_TRUE;
}
