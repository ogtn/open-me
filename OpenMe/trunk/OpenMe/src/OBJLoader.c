//  ome: Open Minimalistic Engine
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


void omeSaveOmeMeshToFile(char *filename, omeMesh *m)
{
    int i, j;
    FILE *file;
    omeBuffer *buffer;
    omeVertexAttrib *attr;

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
        
        // buffer info
        fread(&nbVertices, sizeof nbVertices, 1, file);
        fread(&nbAttributes, sizeof nbAttributes, 1, file);
        fread(&polygonType, sizeof polygonType, 1, file);

        buffer = omeMeshAddBuffer(m, nbVertices, nbAttributes, polygonType);

        for(j = 0; j < buffer->nbAttributes; j++)
        {
            void *data;
            int nbElements;
            omeType type;
            omeBufferType bufferType;

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


// TODO: add an option to swap y and z axis
omeMesh *omeLoadOBJFromFile(char *fileName)
{
    FILE *file;
    char line[1024];
    int nbPositions = 0;
    int nbFaces = 0;
    int currentPosition;
    omeVector *positions;
    omeVector *tmpPositions;
    omeMesh *mesh;
    omeBuffer *buffer;

    if(!(file = fopen(fileName, "r")))
    {
        // TODO: wchar_t ???
        omeLoggerLog("Unable to open the file: %s", fileName);
        return NULL;
    }

    // occurences counting
    // TODO: avoid this with realloc()
    while(fgets(line, 1024, file))
    {
        // position
        if(strstr(line, "v ") == line)
            nbPositions++;

        // position
        else if(strstr(line, "f ") == line)
            nbFaces++;
    }

    rewind(file);
    positions = malloc(nbFaces * 3 * sizeof(omeVector));

    // indices start at 1 in OBJ files, extra empty case at the beginning of the array
    tmpPositions = malloc((nbPositions + 1) * sizeof(omeVector));
    currentPosition = 1;

    while(fgets(line, 1024, file))
    {
        // TODO: skip better than that?
        // skip comments
        if(*line == '#')
            continue;

        // position
        if(strstr(line, "v ") == line)
        {
            sscanf(line, "v %f %f %f", 
                &tmpPositions[currentPosition].x,
                &tmpPositions[currentPosition].y,
                &tmpPositions[currentPosition].z);

            currentPosition++;
        }
    }

    rewind(file);
    currentPosition = 0;

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
            int dummy;
            int a, b, c;

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


            // TODO: three cases in fact: v/vt/vn, v///vn and v/vt/ (but who cares about that one???)
            if(strstr(line, "//"))
                sscanf(line, "f %d//%d %d//%d %d//%d",
                &a, &dummy,
                &b, &dummy,
                &c, &dummy);
            else
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &a, &dummy, &dummy,
                &b, &dummy, &dummy,
                &c, &dummy, &dummy);

            omeVectorCopy(&positions[currentPosition++], &tmpPositions[a]);
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[b]);
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[c]);
        }
    }

    fclose(file);
    //free(tmpPositions);

    mesh = omeMeshCreate(1);
    buffer = omeMeshAddBuffer(mesh, nbFaces * 3, 1, OME_POINTS);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, positions);

    return mesh;
}
