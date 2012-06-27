//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "mesh.h"
#include "geometry.h"
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>


omeMesh *omeMeshCreate(int nbBuffers)
{
    omeMesh *m = calloc(1, sizeof(omeMesh));

    m->nbBuffers = nbBuffers;
    m->geometries = calloc(nbBuffers, sizeof(omeGeometry*));
    m->renderType = OME_VERTEX_GEOMETRY_OBJECT;

    return m;
}


void omeMeshDestroy(omeMesh **m)
{
    //int i;

    //TODO: anything to add here??
    //for(i = 0; i < (*m)->nbBuffers; i++)
    //    omeGeometryDelReference(...);

    memset(*m, 0, sizeof(omeMesh));
    free(*m);
    *m = NULL;
}


//TODO: would omeMeshGetBuffer(geometryIndex...) be better? (on first get, automatically allocate, after ignore parameters)
omeGeometry *omeMeshAddBuffer(omeMesh *m, int nbVertices, int nbAttributes, omePolygonType polygonType)
{
    omeGeometry *g;

    if(m->geometryCpt >= m->nbBuffers)
    {
        omeLoggerLog("All geometries have already been added");
        return NULL;
    }

    g = m->geometries[m->geometryCpt] = omeGeometryCreate(nbVertices, nbAttributes, polygonType, m);
    m->geometryCpt++;

    return g;
}


void omeMeshFinalize(omeMesh *m)
{
    m->finalized = OME_TRUE;
}


void omeMeshBufferFinalized(omeMesh *m)
{
    if(m->nbFinalizedBuffers >= m->nbBuffers)
    {
        omeLoggerLog("All geometries are already finalized\n");
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
    case OME_VERTEX_GEOMETRY_OBJECT:
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
        omeGeometry *g = m->geometries[i];

        glBegin(GL_TRIANGLES);

        for(j = 0; j < g->nbVertices; j++)
        {
            for(k = g->nbAttributes - 1; k >= 0; k--)
            {
                omeVertexAttrib *attr = &g->attributes[j];

                switch (attr->geometryType)
                {
                case OME_ATTRIB_TYPE_POSITION:
                    glVertex3fv((void*)&attr->data[j * omeSizeOf(attr->type)]);
                    break;
                case OME_ATTRIB_TYPE_COLOR:
                    glColor3fv((void*)&attr->data[j * omeSizeOf(attr->type)]);
                    break;
                default:
                    omeLoggerLog("render of desired geometryType not implemented yet\n");
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
        omeGeometry *g = m->geometries[i];

        glBegin(GL_TRIANGLES);

        for(j = g->nbAttributes - 1; j >= 0; j--)
        {
            omeVertexAttrib *attr = &g->attributes[j];
            void *ptr;

            switch (attr->geometryType)
            {
            case OME_ATTRIB_TYPE_POSITION:
                for(k = 0; k < (g->nbVertices * attr->nbElements); k += attr->nbElements)
                {
                    //glColor3fv((void*)&attr->data[k * omeSizeOf(attr->type)]);
                    ptr = &attr->data[k * omeSizeOf(attr->type)];
                    glVertex3fv((void*)&attr->data[k * omeSizeOf(attr->type)]);
                }
                break;
            case OME_ATTRIB_TYPE_COLOR:
                for(k = 0; k < (g->nbVertices * attr->nbElements); k += attr->nbElements)
                {
                    ptr = &attr->data[k * omeSizeOf(attr->type)];
                    glColor3fv((void*)&attr->data[k * omeSizeOf(attr->type)]);
                }
                break;
            default:
                omeLoggerLog("render of desired geometryType not implemented yet\n");
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
        omeGeometryRenderVA(m->geometries[i]);
}


void omeMeshRenderVBO(omeMesh *m)
{
    int i;

    for(i = 0; i < m->nbBuffers; i++)
        omeGeometryRenderVBO(m->geometries[i]);
}


//TODO: add a version number, to avoid failure when loading an obsolete file (maybe a header?)
void omeMeshSave(char *fileName, omeMesh *m)
{
    int i, j;
    FILE *file;
    omeGeometry *geometry;
    omeVertexAttrib *attr;

    if(m->finalized == OME_FALSE)
    {
        omeLoggerLog("Can't save a non finalized mesh\n");
        return;
    }

    file = fopen(fileName, "wb+");

    // mesh info
    fwrite(&m->nbBuffers, sizeof m->nbBuffers, 1, file);

    for(i = 0; i < m->nbBuffers; i++)
    {
        geometry = m->geometries[i];

        // geometry info
        fwrite(&geometry->nbVertices, sizeof geometry->nbVertices, 1, file);
        fwrite(&geometry->nbAttributes, sizeof geometry->nbAttributes, 1, file);
        fwrite(&geometry->polygonType, sizeof geometry->polygonType, 1, file);
        fwrite(&geometry->indexed, sizeof geometry->indexed, 1, file);

        if(geometry->indexed == OME_TRUE)
        {
            attr = &geometry->indices;

            // attribute info
            fwrite(&attr->nbElements, sizeof attr->nbElements, 1, file);
            fwrite(&attr->type, sizeof attr->type, 1, file);
            fwrite(&attr->geometryType, sizeof attr->geometryType, 1, file);

            // attribute data
            fwrite(attr->data, omeSizeOf(attr->type) * attr->nbElements, geometry->nbVertices, file);
        }

        for(j = 0; j < geometry->nbAttributes; j++)
        {
            attr = &geometry->attributes[j];

            // attribute info
            fwrite(&attr->nbElements, sizeof attr->nbElements, 1, file);
            fwrite(&attr->type, sizeof attr->type, 1, file);
            fwrite(&attr->geometryType, sizeof attr->geometryType, 1, file);

            // attribute data
            fwrite(attr->data, omeSizeOf(attr->type) * attr->nbElements, geometry->nbVertices, file);
        }
    }

    fclose(file);
}


omeMesh *omeMeshLoad(char *fileName)
{
    int i, j;
    int nbBuffers;
    FILE *file;
    omeMesh *m;
    omeGeometry *geometry;

    file = fopen(fileName, "rb");
    
    if(file == NULL)
    {
		omeLoggerLog("Unable to load mesh: file %s missing\n", fileName);
		return NULL;
	}

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
        omeAttribType geometryType;

        // geometry info
        fread(&nbVertices, sizeof nbVertices, 1, file);
        fread(&nbAttributes, sizeof nbAttributes, 1, file);
        fread(&polygonType, sizeof polygonType, 1, file);
        fread(&indexed, sizeof indexed, 1, file);

        geometry = omeMeshAddBuffer(m, nbVertices, nbAttributes, polygonType);

        if(indexed == OME_TRUE)
        {
            // attribute info
            fread(&nbElements, sizeof nbElements, 1, file);
            fread(&type, sizeof type, 1, file);
            fread(&geometryType, sizeof geometryType, 1, file);

            // attribute data
            data = malloc(omeSizeOf(type) * nbElements * nbVertices);
            fread(data, omeSizeOf(type) * nbElements, nbVertices, file);

            omeGeometryUseIndices(geometry);
            omeGeometryAddIndices(geometry, type, 0, data);
        }

        for(j = 0; j < geometry->nbAttributes; j++)
        {
            // attribute info
            fread(&nbElements, sizeof nbElements, 1, file);
            fread(&type, sizeof type, 1, file);
            fread(&geometryType, sizeof geometryType, 1, file);

            // attribute data
            data = malloc(omeSizeOf(type) * nbElements * nbVertices);
            fread(data, omeSizeOf(type) * nbElements, nbVertices, file);

            omeGeometryAddAttrib(geometry, nbElements, type, 0, geometryType, data);
        }
    }

    fclose(file);

    return m;
}
