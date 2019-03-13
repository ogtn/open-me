//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "geometry.h"
#include "logger.h"
#include "mesh.h"
#include "engine.h"
#include "scene.h"
#include "program.h"
#include "opengl.h"
#include <stdlib.h>
#include <utlist.h>


static const char *omeAttribNames[] = 
{
    "omeVertexPosition",
    "omeTextCoord0",
    "omeTextCoord1",
    "omeTextCoord2",
    "omeTextCoord3",
    "omeNormal",
    "omeColor",
    "omeIndex",
    "omeUser0",
    "omeUser1",
    "omeUser2",
    "omeUser3"
};


static GLenum omePolygonTypeToGL(omePolygonType polygonType)
{
    switch(polygonType)
    {
    case OME_POINTS: return GL_POINTS;
    case OME_LINE_STRIP: return GL_LINE_STRIP;
    case OME_LINE_LOOP: return GL_LINE_LOOP;
    case OME_LINES: return GL_LINES;
    // case OME_LINE_STRIP_ADJACENCY: return GL_LINE_STRIP_ADJACENCY;
    // case OME_LINES_ADJACENCY: return GL_LINES_ADJACENCY;
    case OME_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
    case OME_TRIANGLE_FAN: return GL_TRIANGLE_FAN;
    case OME_TRIANGLES: return GL_TRIANGLES;
    // case OME_TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
    // case OME_TRIANGLES_ADJACENCY: return GL_TRIANGLES_ADJACENCY;
    // case OME_PATCHES: return GL_PATCHES;
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

    omeSceneAddGeometry(omeEngineGetActiveScene(), g);

    return g;
}


void omeGeometryDestroy(omeGeometry **g)
{
    //TODO: figure out what else do we need to free here -> add flags
    omeDbgClearMem(*g, sizeof(omeGeometry));
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

    if(g->attributes[geometryType].valid)
    {
        omeLoggerLog("This type of attribute has already been added\n");
        return -1;
    }

    attr = &g->attributes[geometryType];
    attr->nbElements = nbElements;
    attr->type = type;
    attr->updateHint = updateHint;
    attr->geometryType = geometryType;
    attr->data = data;
    attr->size = g->nbVertices * nbElements * omeSizeOf(type);
    attr->valid = OME_TRUE;
    attr->name = omeAttribNames[geometryType];
    
    switch(type)
    {
        case OME_FLOAT:
            if(nbElements == 1)
                attr->glslType = "float";
            else if(nbElements == 2)
                attr->glslType = "vec2";
            else if(nbElements == 3)
                attr->glslType = "vec3";
            else
                attr->glslType = "unknown_type";
        break;
        case OME_BYTE:
        case OME_UBYTE:
        case OME_SHORT:
        case OME_USHORT:
        case OME_INT:
        case OME_UINT:
        case OME_DOUBLE:
        default:
        attr->glslType = "unknown_type";
    }

    g->vertexSize += omeSizeOf(type) * nbElements;
    g->size += attr->size;
    g->indexCpt++;

    if(g->indexCpt == g->nbAttributes)
        omeGeometryFinalize(g);

    return geometryType; // TODO: remove this which is useless now I guess...
}


// TODO: think about removing this too, omeGeometryAddAttrib() should be generic now
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


// TODO: check if indexed has indices here...
void omeGeometryFinalize(omeGeometry *g)
{
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

//TODO: implement that :)
/*
void omeGeometryUpdateAttrib(omeGeometry *g, int attribIndex, void *data)
{
    
}
*/

void omeGeometryBuildVBO(omeGeometry *g)
{
    int i;
    int offset = 0;
    omeVertexAttrib *attr;

    if(g->VBOReady == OME_FALSE)
    {
        glGenBuffers(1, &g->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, g->VBO);

        //TODO: use hints here for the last parameter
        glBufferData(GL_ARRAY_BUFFER, g->size, NULL, GL_STATIC_DRAW);
        offset = 0;

        for(i = 0; i < OME_ATTRIB_MAX; i++)
        {
            attr = &g->attributes[i];

            if(attr->valid)
            {
                glBufferSubData(GL_ARRAY_BUFFER, offset, attr->size, attr->data);         
                offset += attr->size;
            }
        }

        g->VBOReady = OME_TRUE;
    }
    else
        omeLoggerLog("VBO already built\n");
}


void omeGeometryEnableAttributes(omeGeometry *g, omeProgram *p)
{
    int i;

    for(i =  0; i < OME_ATTRIB_MAX; i++)
    {
        omeVertexAttrib *attr = &g->attributes[i];

        if(attr->valid)
        {
            attr->loc = omeProgramLocateAttribute(p, attr->name);

            if(attr->loc != -1)
            {
                glEnableVertexAttribArray(attr->loc);
                attr->enabled = OME_TRUE;
                continue;
            }
        }
        
        if(attr->enabled)
        {
            glDisableVertexAttribArray(attr->loc);
            attr->enabled = OME_FALSE;
        }
    }
}


void omeGeometrySendAttributes(omeGeometry *g)
{
    int i;
    int offset = 0;
    omeVertexAttrib *attr;

    // if needed, create VBO and send all vertex attributes
    if(g->VBOReady == OME_FALSE)
        omeGeometryBuildVBO(g);

    glBindBuffer(GL_ARRAY_BUFFER, g->VBO);

    for(i = 0; i < OME_ATTRIB_MAX; i++)
    {
        attr = &g->attributes[i];

        if(attr->enabled)
        {
            switch(attr->type)
            {
            case OME_BYTE:  case OME_UBYTE:
            case OME_SHORT: case OME_USHORT:
            case OME_INT:   case OME_UINT:
                glVertexAttribIPointer(attr->loc, attr->nbElements, omeTypeToGL(attr->type), 0, OME_GEOMETRY_OFFSET(offset));
                break;
            case OME_FLOAT:
                glVertexAttribPointer(attr->loc, attr->nbElements, omeTypeToGL(attr->type), GL_FALSE, 0, OME_GEOMETRY_OFFSET(offset));
                break;
            case OME_DOUBLE:
                glVertexAttribLPointer(attr->loc, attr->nbElements, omeTypeToGL(attr->type), 0, OME_GEOMETRY_OFFSET(offset));
            default:
                omeLoggerLog("Attrib doesn't have a valid data type\n");
                return;
            }

            offset += attr->size;
        }
    }
}


void omeGeometryRender(omeGeometry *g)
{
    if(g->indexed)
        glDrawElements(omePolygonTypeToGL(g->polygonType), g->nbVertices, omeTypeToGL(g->attributes[OME_ATTRIB_TYPE_INDEX].type), g->indices.data);
    else
        glDrawArrays(omePolygonTypeToGL(g->polygonType), 0, g->nbVertices);
}


void omeGeometryDisableAttributes(omeGeometry *g)
{
    int i;

    for(i =  0; i < OME_ATTRIB_MAX; i++)
    {
        omeVertexAttrib *attr = &g->attributes[i];

        if(attr->enabled)
            glDisableVertexAttribArray(attr->loc);
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
