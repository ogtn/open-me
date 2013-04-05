//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "mesh.h"
#include "geometry.h"
#include "logger.h"
#include "color.h"
#include "program.h"
#include "opengl.h"
#include <stdlib.h>


omeMesh *omeMeshCreate(int nbBuffers)
{
    static int meshCpt = 0;
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

    free((*m)->geometries);

    omeDbgClearMem(*m, sizeof(omeMesh));
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
void omeMeshSave(const char *fileName, omeMesh *m)
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


omeMesh *omeMeshLoad(const char *fileName)
{
    int i, j;
    int nbBuffers;
    FILE *file;
    omeMesh *m;
    omeGeometry *geometry;

    file = fopen(fileName, "rb");
    
    if(file == NULL)
    {
        omeLoggerLog("Unable to load mesh %s: file missing\n", fileName);
        return NULL;
    }

    // mesh info
    if(fread(&nbBuffers, sizeof nbBuffers, 1, file) != 1)
    {
        omeLoggerLog("Unable to load mesh %s: missing buffers info\n", fileName);
        goto error;
    }

    m = omeMeshCreate(nbBuffers);

    for(i = 0; i < m->nbBuffers; i++)
    {
        size_t nbVertices;
        int nbAttributes;
        omePolygonType polygonType;
        omeBool indexed;
        void *data;

        // geometry info
        if(fread(&nbVertices, sizeof nbVertices, 1, file) != 1)
        {
            omeLoggerLog("Unable to load mesh %s, buffer %d: missing vertices info\n", fileName, i);
            goto error;
        }

        if(fread(&nbAttributes, sizeof nbAttributes, 1, file) != 1)
        {
            omeLoggerLog("Unable to load mesh %s, buffer %d: missing attributes info\n", fileName, i);
            goto error;
        }

        if(fread(&polygonType, sizeof polygonType, 1, file) != 1)
        {
            omeLoggerLog("Unable to load mesh %s, buffer %d: missing polygon info\n", fileName, i);
            goto error;
        }

        if(fread(&indexed, sizeof indexed, 1, file) != 1)
        {
            omeLoggerLog("Unable to load mesh %s, buffer %d: missing index info\n", fileName, i);
            goto error;
        }

        geometry = omeGeometryCreate(nbVertices, nbAttributes, polygonType);
        omeMeshAddGeometry(m, geometry);

        for(j = 0; j < OME_ATTRIB_MAX; j++)
        {
            omeVertexAttrib attr;
            
            // attribute info
            if(fread(&attr, sizeof attr, 1, file) != 1)
            {
                omeLoggerLog("Unable to load mesh %s, buffer %d, attribute %d: attribute info\n", fileName, i, j);
                goto error;
            }

            // attribute data
            if(attr.actived)
            {
                data = malloc(omeSizeOf(attr.type) * attr.nbElements * nbVertices);
                
                if(fread(data, omeSizeOf(attr.type) * attr.nbElements, nbVertices, file) != nbVertices)
                {
                    omeLoggerLog("Unable to load mesh %s, buffer %d, attribute %d: missing attribute data\n", fileName, i, j);
                    goto error;
                }

                // TODO: an other omeGeometryAddAttrib() wich takes an attrib as parameter?
                omeGeometryAddAttrib(geometry, attr.nbElements, attr.type, 0, attr.geometryType, data);
            }
        }
    }

    fclose(file);

    return m;

    error:
    fclose(file);
    omeMeshDestroy(&m);

    return NULL;
}


omeMesh *omePrimitiveCube(float size, int subdivisions)
{
    int i, j, k, l;
    omeVector *positions, *normals;
    omeVector *pos, *norm;
    int nbVertices;
    omeMesh *m;
    omeGeometry *g;
    float step;
    omeVector xBase, yBase, zBase;
    omeVector xAxis[] = {
        {{0, 0, 0}}, {{0, 1, 0}}, {{0, 1, 1}},
        {{0, 1, 1}}, {{0, 0, 1}}, {{0, 0, 0}}};
    omeVector yAxis[] = {
        {{0, 0, 0}}, {{1, 0, 0}}, {{1, 0, 1}},
        {{1, 0, 1}}, {{0, 0, 1}}, {{0, 0, 0}}};
    omeVector zAxis[] = {
        {{0, 0, 0}}, {{1, 0, 0}}, {{1, 1, 0}},
        {{1, 1, 0}}, {{0, 1, 0}}, {{0, 0, 0}}};
    // normals
    omeVector left = {{-1, 0, 0}};
    omeVector right = {{1, 0, 0}};
    omeVector backward = {{0, -1, 0}};
    omeVector forward = {{0, 1, 0}};
    omeVector down = {{0, 0, -1}};
    omeVector up = {{0, 0, 1}};

    // textsure coordinates
    float coordStep;
    omeVector2 *texCoords;
    omeVector2 *coord;
    omeVector2 coordBase;
    omeVector2 coordVec[] = {
        {{0, 0}}, {{1, 0}}, {{1, 1}},
        {{1, 1}}, {{0, 1}}, {{0, 0}}};

    if(subdivisions < 1)
    {
        omeLoggerLog("You tried to divide by zero, you idiot\n");
        return NULL;
    }

    step = (size / subdivisions);
    coordStep = 1.f / subdivisions;
    nbVertices = subdivisions * subdivisions * 6 * 6;
    pos = positions = malloc(nbVertices * sizeof(omeVector));
    norm = normals = malloc(nbVertices * sizeof(omeVector));
    coord = texCoords = malloc(nbVertices * sizeof(omeVector2));

    for(i = 0; i < 6; i++)
    {
        omeVectorMultScal(&xAxis[i], step, &xAxis[i]);
        omeVectorMultScal(&yAxis[i], step, &yAxis[i]);
        omeVectorMultScal(&zAxis[i], step, &zAxis[i]);
        omeVector2MultScal(&coordVec[i], coordStep, &coordVec[i]);
    }

    for(i = 0; i < subdivisions; i++)
    {
        xBase.y = step * i - size / 2;
        yBase.x = step * i - size / 2;
        zBase.x = step * i - size / 2;
        coordBase.x = coordStep * i;

        for(j = 0; j < subdivisions; j++, pos += 30)
        {
            xBase.z = step * j - size / 2;
            yBase.z = step * j - size / 2;
            zBase.y = step * j - size / 2;
            coordBase.y = coordStep * j;

            for(k = 0; k < 6; k++, pos++, norm++)
            {
                // first triangle
                xBase.x = yBase.y = zBase.z = -size / 2;
                omeVectorAddVector(&xBase, &xAxis[k], pos);
                omeVectorAddVector(&yBase, &yAxis[k], pos + 6);
                omeVectorAddVector(&zBase, &zAxis[k], pos + 12);
                
                // second triangle
                xBase.x = yBase.y = zBase.z = size / 2;
                omeVectorAddVector(&xBase, &xAxis[k], pos + 18);
                omeVectorAddVector(&yBase, &yAxis[k], pos + 24);
                omeVectorAddVector(&zBase, &zAxis[k], pos + 30);

                // normals
                omeVectorCopy(norm, &left);
                omeVectorCopy(norm + 6, &backward);              
                omeVectorCopy(norm + 12, &down);
                omeVectorCopy(norm + 18, &right);
                omeVectorCopy(norm + 24, &forward);
                omeVectorCopy(norm + 30, &up);

                // texture coordinates
                // TODO: fix this: 3 faces have a mirrored texture
                for(l = 0; l < 6; l++)
                    omeVector2AddVector(&coordBase, &coordVec[l], coord++);
            }
        }
    }

    m = omeMeshCreate(1);
    g = omeGeometryCreate(nbVertices, 3, OME_TRIANGLES);
    omeGeometryAddAttrib(g, 3, OME_FLOAT, 0, OME_ATTRIB_TYPE_POSITION, positions);
    omeGeometryAddAttrib(g, 3, OME_FLOAT, 0, OME_ATTRIB_TYPE_NORMAL, normals);
    omeGeometryAddAttrib(g, 2, OME_FLOAT, 0, OME_ATTRIB_TYPE_TEXCOORD_0, texCoords);
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


omeMesh *omePrimitiveGrid(int size, int level)
{
    int i, j;
    omeMesh *m;
    omeGeometry *g;
    omeProgram *program;
    omeVector2 *positions, *pos;
    omeColor *colors, *col;
    omeColor black = {{0, 0, 0, 1}};
    omeColor grey = {{0.5f, 0.5f, 0.5f, 1}};
    float start = - size / 2.f;
    float end = -start;
    int subdivisions = 1 << level;
    float step = size / (float)subdivisions;
    int nbVertices = (subdivisions + 1) * 2 * 2;
    
    pos = positions = malloc(nbVertices * sizeof(omeVector2));
    col = colors = malloc(nbVertices * sizeof(omeColor));

    for(i = 0; i <= subdivisions; i++)
    {
        float current = i * step - size / 2;

        // x axis
        pos->x = current;
        pos->y = start;
        pos++;

        pos->x = current;
        pos->y = end;
        pos++;

        // y axis
        pos->x = start;
        pos->y = current;
        pos++;

        pos->x = end;
        pos->y = current;
        pos++;

        for(j = 0; j < 4; j++)
            *col++ = i == subdivisions / 2 ? black : grey;
    }

    m = omeMeshCreate(1);
    g = omeGeometryCreate(nbVertices, 2, OME_LINES);
    omeGeometryAddAttrib(g, 2, OME_FLOAT, 0, OME_ATTRIB_TYPE_POSITION, positions);
    omeGeometryAddAttrib(g, 4, OME_FLOAT, 0, OME_ATTRIB_TYPE_COLOR, colors);
    omeMeshAddGeometry(m, g);

    program = omeProgramCreate();
    omeProgramAddShader(program, omeShaderLoadFromFile("data/grid.vs"));
    omeProgramAddShader(program, omeShaderLoadFromFile("data/grid.ps"));
    omeProgramLink(program);
    m->program = program;

    return m;
}
