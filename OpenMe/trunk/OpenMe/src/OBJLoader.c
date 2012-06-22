﻿//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "OBJLoader.h"
#include "mesh.h"
#include "buffer.h"
#include "logger.h"
#include "vector.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//TODO: move omeSaveOmeMeshToFile() and omeLoadOmeMeshFromFile() from this file
//TODO: add a version number, to avoid failure when loading an obsolete file (maybe a header?)
void omeSaveOmeMeshToFile(char *filename, omeMesh *m)
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


omeMesh *omeLoadOmeMeshFromFile(char *filename)
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


omeMesh *omeLoadOBJFromFile(char *fileName, omeBool swapYZ)
{
    FILE *file;
    char line[1024];
    int nbPositions = 0;
    int nbNormals = 0;
    int nbTexCoords = 0;
    int nbFaces = 0;
    int currentPosition;
    int currentNormal;
    int currentTexCoord;
    omeVector *positions;
    omeVector *normals;
    omeVector2 *texCoords;
    omeVector *tmpPositions;
    omeVector *tmpNormals;
    omeVector2 *tmpTexCoords;
    omeMesh *mesh;
    omeBuffer *buffer;

    file = fopen(fileName, "r");

    if(file == NULL)
    {
        // TODO: wchar_t ???
        omeLoggerLog("Unable to open the file: %s", fileName);
        return NULL;
    }

    // first pass: count occurences
    // TODO: avoid this with realloc()
    while(fgets(line, 1024, file))
    {
        // position
        if(strstr(line, "v ") == line)
            nbPositions++;

        // texture coordinate
        else if(strstr(line, "vt ") == line)
            nbTexCoords++;

        // normal
        else if(strstr(line, "vn ") == line)
            nbNormals++;

        // face
        else if(strstr(line, "f ") == line)
            nbFaces++;
    }

    // buffers allocations
    // TODO: do this after tmp buffers have been filled and compressed to improve performances and memory used?
    positions = malloc(nbFaces * 3 * sizeof(omeVector));
    texCoords = malloc(nbFaces * 3 * sizeof(omeVector2));
    normals = malloc(nbFaces * 3 * sizeof(omeVector));

    // indices start at 1 in OBJ files, need an extra empty case at the beginning of the array
    tmpPositions = malloc((nbPositions + 1) * sizeof(omeVector));
    tmpTexCoords = malloc((nbTexCoords + 1) * sizeof(omeVector2));
    tmpNormals = malloc((nbNormals + 1) * sizeof(omeVector));

    currentPosition = 1;
    currentTexCoord = 1;
    currentNormal = 1;
    rewind(file);

    // second pass: load vertex attributes in temporary buffers
    while(fgets(line, 1024, file))
    {
        // TODO: skip better than that?
        // skip comments
        if(*line == '#')
            continue;

        // positions
        if(strstr(line, "v ") == line)
        {
            if(swapYZ)
                sscanf(line, "v %f %f %f", 
                &tmpPositions[currentPosition].x,
                &tmpPositions[currentPosition].z,
                &tmpPositions[currentPosition].y);
            else
                sscanf(line, "v %f %f %f", 
                &tmpPositions[currentPosition].x,
                &tmpPositions[currentPosition].y,
                &tmpPositions[currentPosition].z);

            currentPosition++;
        }

        // texture coordinates
        else if(strstr(line, "vt ") == line)
        {
            sscanf(line, "vt %f %f", 
                &tmpTexCoords[currentTexCoord].x,
                &tmpTexCoords[currentTexCoord].y);

            currentTexCoord++;
        }

        // normals
        else if(strstr(line, "vn ") == line)
        {
            if(swapYZ)
                sscanf(line, "vn %f %f %f", 
                &tmpNormals[currentNormal].x,
                &tmpNormals[currentNormal].z,
                &tmpNormals[currentNormal].y);
            else
                sscanf(line, "vn %f %f %f", 
                &tmpNormals[currentNormal].x,
                &tmpNormals[currentNormal].y,
                &tmpNormals[currentNormal].z);

            currentNormal++;
        }
    }

    currentPosition = 0;
    currentTexCoord = 0;
    currentNormal = 0;
    rewind(file);

    // third pass: load faces indices
    // TODO: do this in a temporary buffer during the second pass
    while(fgets(line, 1024, file))
    {
        // TODO: skip better than that?
        // skip comments
        if(*line == '#')
            continue;

        // face
        if(strstr(line, "f ") == line)
        {
            char *slash;
            int nbSlashes = 0;
            int v1, v2, v3;
            int vt1, vt2, vt3;
            int vn1, vn2, vn3;

            // count slashes
            slash = strchr(line, '/');
            while(slash)
            {
                slash = strchr(slash + 1, '/');
                nbSlashes++;
            }

            if(nbSlashes != 6)
            {
                omeLoggerLog("OBJ loader failed, faces are not triangles?");
                return NULL;
            }

            // TODO: three cases in fact: v/vt/vn, v//vn and v/vt/ (but who cares about that one???)
            if(strstr(line, "//"))
                sscanf(line, "f %d//%d %d//%d %d//%d",
                &v1, &vn1,
                &v2, &vn2,
                &v3, &vn3);
            else
            {
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &v1, &vt1, &vn1,
                &v2, &vt2, &vn2,
                &v3, &vt3, &vn3);

                // texture coordinates
                omeVector2Copy(&texCoords[currentTexCoord++], &tmpTexCoords[vt1]);
                omeVector2Copy(&texCoords[currentTexCoord++], &tmpTexCoords[vt2]);
                omeVector2Copy(&texCoords[currentTexCoord++], &tmpTexCoords[vt3]);
            }

            // positions
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[v1]);
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[v2]);
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[v3]);

            // normals
            omeVectorCopy(&normals[currentNormal++], &tmpNormals[vn1]);
            omeVectorCopy(&normals[currentNormal++], &tmpNormals[vn2]);
            omeVectorCopy(&normals[currentNormal++], &tmpNormals[vn3]);
        }
    }

    fclose(file);
    free(tmpPositions);
    free(tmpTexCoords);
    free(tmpNormals);

    mesh = omeMeshCreate(1);
    buffer = omeMeshAddBuffer(mesh, nbFaces * 3, 3, OME_TRIANGLES);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, positions);
    omeBufferAddAttrib(buffer, 2, OME_FLOAT, 0, OME_BUFFER_TYPE_TEXCOORD_0, texCoords);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_NORMAL, normals);

    return mesh;
}
