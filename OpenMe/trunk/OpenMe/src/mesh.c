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
    m->renderType = OME_VERTEX_BUFFER_OBJECT;

    return m;
}


void omeMeshDestroy(omeMesh **m)
{
    //int i;

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
    if(m->finalized == OME_FALSE)
    {
        omeLoggerLog("Can't render a non finalized mesh\n");
        return;
    }

    switch(m->renderType)
    {
    case OME_IMMEDIATE:
        omeMeshRenderImmediate(m); 
        break;
    case OME_VERTEX_ARRAY:
        omeMeshRenderVA(m);
        break;
    case OME_VERTEX_BUFFER_OBJECT:
        omeMeshRenderVBO(m);
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


void omeMeshRenderVBO(omeMesh *m)
{
    int i;

    for(i = 0; i < m->nbBuffers; i++)
        omeBufferRenderVBO(m->buffers[i]);
}


//TODO: add a version number, to avoid failure when loading an obsolete file (maybe a header?)
void omeMeshSave(char *filename, omeMesh *m)
{
    int i, j;
    FILE *file;
    omeBuffer *buffer;
    omeVertexAttrib *attr;

    if(m->finalized == OME_FALSE)
    {
        omeLoggerLog("Can't save a non finalized mesh\n");
        return;
    }

    file = fopen(filename, "wb+");

    // mesh info
    fwrite(&m->nbBuffers, sizeof m->nbBuffers, 1, file);

    for(i = 0; i < m->nbBuffers; i++)
    {
        buffer = m->buffers[i];

        // buffer info
        fwrite(&buffer->nbVertices, sizeof buffer->nbVertices, 1, file);
        fwrite(&buffer->nbAttributes, sizeof buffer->nbAttributes, 1, file);
        fwrite(&buffer->polygonType, sizeof buffer->polygonType, 1, file);
        fwrite(&buffer->indexed, sizeof buffer->indexed, 1, file);

        if(buffer->indexed == OME_TRUE)
        {
            attr = &buffer->indices;

            // attribute info
            fwrite(&attr->nbElements, sizeof attr->nbElements, 1, file);
            fwrite(&attr->type, sizeof attr->type, 1, file);
            fwrite(&attr->bufferType, sizeof attr->bufferType, 1, file);

            // attribute data
            fwrite(attr->data, omeSizeOf(attr->type) * attr->nbElements, buffer->nbVertices, file);
        }

        for(j = 0; j < buffer->nbAttributes; j++)
        {
            attr = &buffer->attributes[j];

            // attribute info
            fwrite(&attr->nbElements, sizeof attr->nbElements, 1, file);
            fwrite(&attr->type, sizeof attr->type, 1, file);
            fwrite(&attr->bufferType, sizeof attr->bufferType, 1, file);

            // attribute data
            fwrite(attr->data, omeSizeOf(attr->type) * attr->nbElements, buffer->nbVertices, file);
        }
    }

    fclose(file);
}


omeMesh *omeMeshLoad(char *filename)
{
    int i, j;
    int nbBuffers;
    FILE *file;
    omeMesh *m;
    omeBuffer *buffer;

    file = fopen(filename, "rb");

    // mesh info
    fread(&nbBuffers, sizeof nbBuffers, 1, file);
    m = omeMeshCreate(nbBuffers);

    for(i = 0; i < m->nbBuffers; i++)
    {
        int nbVertices;
        int nbAttributes;
        omePolygonType polygonType;
        omeBool indexed;
        void *data;
        int nbElements;
        omeType type;
        omeBufferType bufferType;

        // buffer info
        fread(&nbVertices, sizeof nbVertices, 1, file);
        fread(&nbAttributes, sizeof nbAttributes, 1, file);
        fread(&polygonType, sizeof polygonType, 1, file);
        fread(&indexed, sizeof indexed, 1, file);

        buffer = omeMeshAddBuffer(m, nbVertices, nbAttributes, polygonType);

        if(indexed == OME_TRUE)
        {
            // attribute info
            fread(&nbElements, sizeof nbElements, 1, file);
            fread(&type, sizeof type, 1, file);
            fread(&bufferType, sizeof bufferType, 1, file);

            // attribute data
            data = malloc(omeSizeOf(type) * nbElements * nbVertices);
            fread(data, omeSizeOf(type) * nbElements, nbVertices, file);

            omeBufferUseIndices(buffer);
            omeBufferAddIndices(buffer, type, 0, data);
        }

        for(j = 0; j < buffer->nbAttributes; j++)
        {
            // attribute info
            fread(&nbElements, sizeof nbElements, 1, file);
            fread(&type, sizeof type, 1, file);
            fread(&bufferType, sizeof bufferType, 1, file);

            // attribute data
            data = malloc(omeSizeOf(type) * nbElements * nbVertices);
            fread(data, omeSizeOf(type) * nbElements, nbVertices, file);

            omeBufferAddAttrib(buffer, nbElements, type, 0, bufferType, data);
        }
    }

    fclose(file);

    return m;
}