//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "geometry.h"
#include "logger.h"
#include "mesh.h"
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <utlist.h>


GLenum omePolygonTypeToGL(omePolygonType polygonType)
{
    switch(polygonType)
    {
    case OME_POINTS: return GL_POINTS;
    case OME_LINE_STRIP: return GL_LINE_STRIP;
    case OME_LINE_LOOP: return GL_LINE_LOOP;
    case OME_LINES: return GL_LINES;
    case OME_LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
    case OME_LINES_ADJACENCY: return GL_LINES_ADJACENCY;
    case OME_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case OME_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
    case OME_TRIANGLES: return GL_TRIANGLES;
    case OME_TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
    case OME_TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
    case OME_PATCHES: return GL_PATCHES;
    default: omeLoggerLog("Invalid polygon type");
    }

    return GL_POINTS;
}


omeGeometry *omeGeometryCreate(int nbVertices, int nbAttributes, omePolygonType polygonType)
{
    omeGeometry *g = calloc(1, sizeof(omeGeometry));

    // default values 
    //TODO: add setters to modifiy them or even better -> self adapted solution based on a benchmark :D
    g->indexed = OME_FALSE;
    g->interleaved = OME_FALSE;
    g->padded = OME_FALSE;

    g->nbVertices = nbVertices;
    g->nbAttributes = nbAttributes;
    g->polygonType = polygonType;

    return g;
}


void omeGeometryDestroy(omeGeometry **g)
{
    int i = 0;

    //TODO: figure out what else do we need to free here -> add flags

    memset(*g, 0, sizeof(omeGeometry));
    free(*g);
    *g = NULL;
}


int omeGeometryAddAttrib(omeGeometry *g, int nbElements, omeType type, int updateHint, omeAttribType geometryType, void *data)
{
    omeVertexAttrib *attr;

    if(g->indexCpt >= g->nbAttributes)
    {
        omeLoggerLog("All attributes have already been added");
        return -1;
    }

    // TODO: play with indices to automatize that, or add a special function for position??? (check opengl 4.2 documentation)
    if((g->indexCpt == 0 && geometryType != OME_ATTRIB_TYPE_POSITION) ||
        (geometryType == OME_ATTRIB_TYPE_POSITION && g->indexCpt != 0))
    {
        omeLoggerLog("vertex position must be added first");
        return -1;
    }

    attr = &g->attributes[g->indexCpt];
    attr->nbElements = nbElements;
    attr->type = type;
    attr->updateHint = updateHint;
    attr->geometryType = geometryType;
    attr->data = data;
    attr->size = g->nbVertices * nbElements * omeSizeOf(type);

    g->vertexSize += omeSizeOf(type) * nbElements;
    g->size += attr->size;
    g->indexCpt++;

    if(g->indexCpt == g->nbAttributes)
        omeGeometryFinalize(g);

    return g->indexCpt;
}


int omeGeometryAddIndices(omeGeometry *g, omeType type, int updateHint, void *data)
{
    omeVertexAttrib *attr;

    if(g->indexed == OME_FALSE)
    {
        omeLoggerLog("This mesh is not indexed");
        return -1;
    }

    attr = &g->indices;
    attr->nbElements = 1;
    attr->type = type;
    attr->updateHint = updateHint;
    attr->geometryType = OME_ATTRIB_TYPE_INDEX;
    attr->data = data;
    attr->size = g->nbVertices * omeSizeOf(type);

    return g->nbAttributes;
}


void omeGeometryFinalize(omeGeometry *g)
{
    int i;
    char *ptr;

    // TODO: remove thoses tests once the function will be private?
    if(g->indexCpt != g->nbAttributes)
    {
        omeLoggerLog("Can't finalize, attributes are missing");
        return;
    }

    if(g->finalized)
    {
        omeLoggerLog("Allready finalized");
        return;
    }

    //TODO: take the hints and the options into account somewhere around here

    //if(g->mesh != NULL)
    //    omeMeshBufferFinalized(g->mesh);

    g->finalized = OME_TRUE;
}


void omeGeometryUpdateAttrib(omeGeometry *g, int attribIndex, void *data)
{
    //TODO: implement that :)
}


void omeGeometryRenderVA(omeGeometry *g)
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

    for(i = 0; i < g->nbAttributes; i++)
    {
        attr = &g->attributes[i];

        if(attr->geometryType == OME_ATTRIB_TYPE_POSITION)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else if(attr->geometryType == OME_ATTRIB_TYPE_COLOR)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else if(attr->geometryType == OME_ATTRIB_TYPE_NORMAL)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, attr->data);
        }
        else if(attr->geometryType == OME_ATTRIB_TYPE_TEXCOORD_0)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(attr->nbElements, GL_FLOAT, 0, attr->data);
        }
        else
        {
            omeLoggerLog("Not implemented yet\n");
            return; //TODO: find a way to not skip the rest (goto??)
        }
    }

    if(g->indexed)
    {
        if(g->indices.type != OME_UBYTE)
        {
            omeLoggerLog("Not implemented yet\n");
            return;
        }

        glDrawElements(omePolygonTypeToGL(g->polygonType), g->nbVertices, GL_UNSIGNED_BYTE, g->indices.data);
    }
    else
        glDrawArrays(omePolygonTypeToGL(g->polygonType), 0, g->nbVertices);

    for(i = 0; i < g->nbAttributes; i++)
    {
        attr = &g->attributes[i];

        if(attr->geometryType == OME_ATTRIB_TYPE_POSITION)
            glDisableClientState(GL_VERTEX_ARRAY);
        else if(attr->geometryType == OME_ATTRIB_TYPE_COLOR)
            glDisableClientState(GL_COLOR_ARRAY);
        else if(attr->geometryType == OME_ATTRIB_TYPE_NORMAL)
            glDisableClientState(GL_NORMAL_ARRAY);
        else if(attr->geometryType == OME_ATTRIB_TYPE_TEXCOORD_0)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}


void omeGeometryRenderVBO(omeGeometry *g)
{
    int i;
    int offset = 0;
    omeVertexAttrib *attr;

    // if needed, create VBO and send all vertex attributes
    if(g->VBOReady == OME_FALSE)
    {
        glGenBuffers(1, &g->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, g->VBO);

        //TODO: use hints here for the last parameter
        glBufferData(GL_ARRAY_BUFFER, g->size, NULL, GL_STATIC_DRAW);
        offset = 0;

         for(i = 0; i < g->nbAttributes; i++)
         {
             attr = &g->attributes[i];

             glBufferSubData(GL_ARRAY_BUFFER, offset, attr->size, attr->data);         
             offset += attr->size;
         }

         g->VBOReady = OME_TRUE;
    }

    glBindBuffer(GL_ARRAY_BUFFER, g->VBO);
    offset = 0;

    for(i = 0; i < g->nbAttributes ; i++)
    {
        attr = &g->attributes[i];

        if(attr->geometryType == OME_ATTRIB_TYPE_POSITION)
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(attr->nbElements, GL_FLOAT, 0, OME_GEOMETRY_OFFSET(offset));
        }
        else if(attr->geometryType == OME_ATTRIB_TYPE_COLOR)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(attr->nbElements, GL_FLOAT, 0, OME_GEOMETRY_OFFSET(offset));
        }
        else if(attr->geometryType == OME_ATTRIB_TYPE_NORMAL)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, 0, OME_GEOMETRY_OFFSET(offset));
        }
        else if(attr->geometryType == OME_ATTRIB_TYPE_TEXCOORD_0)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(attr->nbElements, GL_FLOAT, 0, OME_GEOMETRY_OFFSET(offset));
        }
        else
        {
            omeLoggerLog("Not implemented yet\n");
            return; //TODO: find a way to not skip the rest (goto??)
        }

        offset += attr->size;
    }

    if(g->indexed)
    {
        if(g->indices.type != OME_UBYTE)
        {
            omeLoggerLog("Not implemented yet\n");
            return;
        }

        glDrawElements(omePolygonTypeToGL(g->polygonType), g->nbVertices, GL_UNSIGNED_BYTE, g->indices.data);
    }
    else
        glDrawArrays(omePolygonTypeToGL(g->polygonType), 0, g->nbVertices);

    for(i =  0; i < g->nbAttributes; i++)
    {
        attr = &g->attributes[i];

        if(attr->geometryType == OME_ATTRIB_TYPE_POSITION)
            glDisableClientState(GL_VERTEX_ARRAY);
        else if(attr->geometryType == OME_ATTRIB_TYPE_COLOR)
            glDisableClientState(GL_COLOR_ARRAY);
        else if(attr->geometryType == OME_ATTRIB_TYPE_NORMAL)
            glDisableClientState(GL_NORMAL_ARRAY);
        else if(attr->geometryType == OME_ATTRIB_TYPE_TEXCOORD_0)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}


void omeGeometryUseIndices(omeGeometry *g)
{
    g->indexed = OME_TRUE;
}


void omeGeometryAddRef(omeGeometry *g, omeMesh *m)
{
    omeMeshListElement *element = calloc(1, sizeof(omeMeshListElement));
    element->mesh = m;
    DL_APPEND(g->references, element);

    g->nbReferences++;
}


void omeGeometryDelRef(omeGeometry *g, omeMesh *m)
{
    omeMeshListElement *element = NULL;

    DL_SEARCH_SCALAR(g->references, element, mesh, m);

    if(element)
    {
        DL_DELETE(g->references, element);
        g->nbReferences--;

        // TODO: add a flag to disable this, to allow static arrays
        if(g->nbReferences == 0)
            omeGeometryDestroy(&g);
    }
    else
        omeLoggerLog("This geometry wasn't referenced by this mesh\n");
}
