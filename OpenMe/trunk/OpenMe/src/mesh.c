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


static int meshCpt = 0;


omeMesh *omeMeshCreate(int nbBuffers)
{
    char name[OME_NAME_MAXLEN];
    omeMesh *m = calloc(1, sizeof(omeMesh));

    m->nbBuffers = nbBuffers;
    m->geometries = calloc(nbBuffers, sizeof(omeGeometry*));

    sprintf(name, "mesh_%d", meshCpt++);
    omeEntityInit(m, OME_ENTITY_MESH, name);

    return m;
}


void omeMeshDestroy(omeMesh **m)
{
    int i;

    for(i = 0; i < (*m)->nbBuffers; i++)
        omeGeometryDelRef((*m)->geometries[i], *m);

    memset(*m, 0, sizeof(omeMesh));
    free(*m);
    *m = NULL;
}


omeGeometry *omeMeshAddGeometry(omeMesh *m, omeGeometry *g)
{
    if(m->geometryCpt >= m->nbBuffers)
    {
        omeLoggerLog("All geometries have already been added");
        return NULL;
    }

    g = m->geometries[m->geometryCpt] = g;
    omeGeometryAddRef(g, m);
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


//TODO: add a version number, to avoid failure when loading an obsolete file (maybe a header?)
void omeMeshSave(char *fileName, omeMesh *m)
{
    int i, j;
    FILE *file;
    omeGeometry *geometry;
    omeVertexAttrib *attr;

    // TODO: fix the finalizing process of meshes and geometries
    /* 
    if(m->finalized == OME_FALSE)
    {
        omeLoggerLog("Can't save a non finalized mesh\n");
        return;
    }
    */

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

        for(j = 0; j < OME_ATTRIB_MAX; j++)
        {
            attr = &geometry->attributes[j];

            // attribute info
            fwrite(attr, sizeof *attr, 1, file);

            // attribute data
            if(attr->actived)
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

        // geometry info
        fread(&nbVertices, sizeof nbVertices, 1, file);
        fread(&nbAttributes, sizeof nbAttributes, 1, file);
        fread(&polygonType, sizeof polygonType, 1, file);
        fread(&indexed, sizeof indexed, 1, file);

        geometry = omeGeometryCreate(nbVertices, nbAttributes, polygonType);
        omeMeshAddGeometry(m, geometry);

        for(j = 0; j < OME_ATTRIB_MAX; j++)
        {
            omeVertexAttrib attr;
            
            // attribute info
            fread(&attr, sizeof attr, 1, file);

            // attribute data
            if(attr.actived)
            {
                data = malloc(omeSizeOf(attr.type) * attr.nbElements * nbVertices);
                fread(data, omeSizeOf(attr.type) * attr.nbElements, nbVertices, file);

                // TODO: an other omeGeometryAddAttrib() wich takes an attrib as parameter?
                omeGeometryAddAttrib(geometry, attr.nbElements, attr.type, 0, attr.geometryType, data);
            }
        }
    }

    fclose(file);

    return m;
}


omeMesh *omePrimitiveCube(float size, int subdivisions)
{
    int i, j, k;
    omeVector *positions, *normals;
    omeVector *pos, *norm;
    int nbVertices;
    omeMesh *m;
    omeGeometry *g;
    float step;
    omeVector xBase, yBase, zBase;
    omeVector xAxis[] = {
        {0, 0, 0}, {0, 1, 0}, {0, 1, 1},
        {0, 1, 1}, {0, 0, 1}, {0, 0, 0}};
    omeVector yAxis[] = {
        {0, 0, 0}, {1, 0, 0}, {1, 0, 1},
        {1, 0, 1}, {0, 0, 1}, {0, 0, 0}};
    omeVector zAxis[] = {
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0},
        {1, 1, 0}, {0, 1, 0}, {0, 0, 0}};
    // normals
    omeVector left = {-1, 0, 0};
    omeVector right = {1, 0, 0};
    omeVector backward = {0, -1, 0};
    omeVector forward = {0, 1, 0};
    omeVector down = {0, 0, -1};
    omeVector up = {0, 0, 1};

    if(subdivisions < 1)
    {
        omeLoggerLog("You tried to divide by zero, you idiot\n");
        return NULL;
    }

    step = (size / subdivisions);
    nbVertices = subdivisions * subdivisions * 6 * 6;
    pos = positions = malloc(nbVertices * sizeof(omeVector));
    norm = normals = malloc(nbVertices * sizeof(omeVector));

    for(i = 0; i < 6; i++)
    {
        omeVectorMultScal(&xAxis[i], step, &xAxis[i]);
        omeVectorMultScal(&yAxis[i], step, &yAxis[i]);
        omeVectorMultScal(&zAxis[i], step, &zAxis[i]);
    }

    for(i = 0; i < subdivisions; i++)
    {
        xBase.y = step * i - size / 2;
        yBase.x = step * i - size / 2;
        zBase.x = step * i - size / 2;

        for(j = 0; j < subdivisions; j++)
        {
            xBase.z = step * j - size / 2;
            yBase.z = step * j - size / 2;
            zBase.y = step * j - size / 2;

            // left
            xBase.x = -size / 2;
            for(k = 0; k < 6; k++)
            {
                omeVectorAddVector(&xBase, &xAxis[k], pos++);
                omeVectorCopy(norm++, &left);
            }

            // right
            xBase.x = size / 2;
            for(k = 0; k < 6; k++)
            {
                omeVectorAddVector(&xBase, &xAxis[k], pos++);
                omeVectorCopy(norm++, &right);
            }

            // backward
            yBase.y = -size / 2;
            for(k = 0; k < 6; k++)
            {
                omeVectorAddVector(&yBase, &yAxis[k], pos++);
                omeVectorCopy(norm++, &backward);
            }

            // forward
            yBase.y = size / 2;
            for(k = 0; k < 6; k++)
            {
                omeVectorAddVector(&yBase, &yAxis[k], pos++);
                omeVectorCopy(norm++, &forward);
            }

            // bottom
            zBase.z = -size / 2;
            for(k = 0; k < 6; k++)
            {
                omeVectorAddVector(&zBase, &zAxis[k], pos++);
                omeVectorCopy(norm++, &down);
            }
            
            // top
            zBase.z = size / 2;
            for(k = 0; k < 6; k++)
            {
                omeVectorAddVector(&zBase, &zAxis[k], pos++);
                omeVectorCopy(norm++, &up);
            }
        }
    }

    m = omeMeshCreate(2);
    g = omeGeometryCreate(nbVertices, 2, OME_TRIANGLES);
    omeGeometryAddAttrib(g, 3, OME_FLOAT, 0, OME_ATTRIB_TYPE_POSITION, positions);
    omeGeometryAddAttrib(g, 3, OME_FLOAT, 0, OME_ATTRIB_TYPE_NORMAL, normals);
    omeMeshAddGeometry(m, g);

    return m;
}


omeMesh *omePrimitiveSphere(float radius, int subdivisions)
{
    int i;
    omeMesh *m;
    int nbVertices;
    omeVector *pos, *norm;

    m = omePrimitiveCube(radius, subdivisions);
    nbVertices = m->geometries[0]->nbVertices;
    pos = m->geometries[0]->attributes[OME_ATTRIB_TYPE_POSITION].data;
    norm = m->geometries[0]->attributes[OME_ATTRIB_TYPE_NORMAL].data;

    for(i = 0; i < nbVertices; i++)
    {
        omeVectorMultScal(pos, radius / omeVectorLength(pos) * 0.5f, pos);
        *norm = *pos;
        omeVectorNormalize(norm);

        pos++;
        norm++;
    }

    return m;
}
