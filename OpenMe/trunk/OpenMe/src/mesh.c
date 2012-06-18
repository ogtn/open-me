//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "mesh.h"
#include "buffer.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>


omeMesh *omeMeshCreate(int nbBuffers)
{
    omeMesh *m = calloc(1, sizeof(omeMesh));

    m->nbBuffers = nbBuffers;
    m->buffers = calloc(nbBuffers, sizeof(omeBuffer*));
    m->renderType = OME_VERTEX_ARRAY;

    return m;
}


void omeMeshDestroy(omeMesh **m)
{
    int i;

    //TODO: anything to add here??
    //for(i = 0; i < (*m)->nbBuffers; i++)
    //    omeBufferDelReference(...);

    memset(*m, 0, sizeof(omeMesh));
    free(*m);
    *m = NULL;
}


//TODO: would omeMeshGetBuffer(bufferIndex...) be better? (on first get, automatically allocate, after ignore parameters)
omeBuffer *omeMeshAddBuffer(omeMesh *m, int nbVertices, int nbAttributes, omePolygonType polygonType)
{
    omeBuffer *b;

    if(m->bufferCpt >= m->nbBuffers)
    {
        omeLoggerLog("All buffers have already been added");
        return NULL;
    }

    b = m->buffers[m->bufferCpt] = omeBufferCreate(nbVertices, nbAttributes, polygonType, m);
    m->bufferCpt++;

    return b;
}


void omeMeshFinalize(omeMesh *m)
{
    m->finalized = OME_TRUE;
}


void omeMeshBufferFinalized(omeMesh *m)
{
    if(m->nbFinalizedBuffers >= m->nbBuffers)
    {
        omeLoggerLog("All buffers are already finalized\n");
        return;
    }

    m->nbFinalizedBuffers++;

    if(m->nbFinalizedBuffers == m->nbBuffers)
        omeMeshFinalize(m);
}


void omeMeshRender(omeMesh *m)
{
    switch(m->renderType)
    {
    case OME_IMMEDIATE:
        omeMeshRenderImmediate(m); 
        break;
    case OME_VERTEX_ARRAY:
        omeMeshRenderVA(m);
        break;
    default:
        omeLoggerLog("Not implemented yet\n");
    }
}


void omeMeshRenderImmediate(omeMesh *m)
{
    int i, j, k;

    for(i = 0; i < m->nbBuffers; i++)
    {
        omeBuffer *b = m->buffers[i];

        glBegin(GL_TRIANGLES);

        for(j = 0; j < b->nbVertices; j++)
        {
            for(k = b->nbAttributes - 1; k >= 0; k--)
            {
                omeVertexAttrib *attr = &b->attributes[j];

                switch (attr->bufferType)
                {
                case OME_BUFFER_TYPE_POSITION:
                    glVertex3fv((void*)&attr->data[j * omeSizeOf(attr->type)]);
                    break;
                case OME_BUFFER_TYPE_COLOR:
                    glColor3fv((void*)&attr->data[j * omeSizeOf(attr->type)]);
                    break;
                default:
                    omeLoggerLog("render of desired bufferType not implemented yet\n");
                    break;
                }
            }
        }

        glEnd();
    }
}


void omeMeshRenderImmediate_old(omeMesh *m)
{
    int i, j, k;

    for(i = 0; i < m->nbBuffers; i++)
    {
        omeBuffer *b = m->buffers[i];

        glBegin(GL_TRIANGLES);

        for(j = b->nbAttributes - 1; j >= 0; j--)
        {
            omeVertexAttrib *attr = &b->attributes[j];
            void *ptr;

            switch (attr->bufferType)
            {
            case OME_BUFFER_TYPE_POSITION:
                for(k = 0; k < (b->nbVertices * attr->nbElements); k += attr->nbElements)
                {
                    //glColor3fv((void*)&attr->data[k * omeSizeOf(attr->type)]);
                    ptr = &attr->data[k * omeSizeOf(attr->type)];
                    glVertex3fv((void*)&attr->data[k * omeSizeOf(attr->type)]);
                }
                break;
            case OME_BUFFER_TYPE_COLOR:
                for(k = 0; k < (b->nbVertices * attr->nbElements); k += attr->nbElements)
                {
                    ptr = &attr->data[k * omeSizeOf(attr->type)];
                    glColor3fv((void*)&attr->data[k * omeSizeOf(attr->type)]);
                }
                break;
            default:
                omeLoggerLog("render of desired bufferType not implemented yet\n");
                break;
            }
        }

        glEnd();
    }
}


void omeMeshRenderVA(omeMesh *m)
{
    int i;

    for(i = 0; i < m->nbBuffers; i++)
        omeBufferRenderVA(m->buffers[i]);
}
