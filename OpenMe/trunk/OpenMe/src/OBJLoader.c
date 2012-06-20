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


omeMesh *omeLoadOBJFromFile(wchar_t *fileName)
{
    FILE *file;
    char line[1024];
    int nbPositions = 0;
    int nbFaces = 0;
    int currentPosition = 0;
    omeVector *positions;
    omeVector *tmpPositions;
    omeMesh *mesh;
    omeBuffer *buffer;
    
    if(!(file = _wfopen(fileName, L"r")))
    {
        // TODO: wchar_t !!!
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
    tmpPositions = malloc(nbPositions * sizeof(omeVector));
    positions = malloc(nbFaces * 3 * sizeof(omeVector));

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
            char *slash = line;
            int nbSlashes = 0;
            int dummy;
            int a, b, c;

            //while(slash = strchr(line, '/'))
            //    nbSlashes++;

            //if(nbSlashes != 6)
            //{
            //    omeLoggerLog("OBJ loader failed, faces are not triangles?");
            //    return NULL;
            //}

            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &a, &dummy, &dummy,
                &b, &dummy, &dummy,
                &c, &dummy, &dummy);

            // those fuckin' OBJ indices start at 1...
            // TODO: add an empty space in the array instead
            a--;
            b--;
            c--;

            omeVectorCopy(&positions[currentPosition++], &tmpPositions[a]);
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[b]);
            omeVectorCopy(&positions[currentPosition++], &tmpPositions[c]);
        }
    }

    fclose(file);
    //free(tmpPositions);

    mesh = omeMeshCreate(1);
    buffer = omeMeshAddBuffer(mesh, nbFaces * 3, 1, OME_TRIANGLES​);
    omeBufferAddAttrib(buffer, 3, OME_FLOAT, 0, OME_BUFFER_TYPE_POSITION, positions);

    return mesh;
}
