//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_GEOMETRY_H
#define OME_GEOMETRY_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"


// forward declaration
typedef struct omeMesh omeMesh;
typedef struct omeMeshListElement omeMeshListElement, omeMeshList;
typedef struct omeProgram omeProgram;

typedef enum omeAttribType
{
    OME_ATTRIB_TYPE_POSITION,
    OME_ATTRIB_TYPE_TEXCOORD_0,
    OME_ATTRIB_TYPE_TEXCOORD_1,
    OME_ATTRIB_TYPE_TEXCOORD_2,
    OME_ATTRIB_TYPE_TEXCOORD_3,
    OME_ATTRIB_TYPE_NORMAL,
    OME_ATTRIB_TYPE_COLOR,
    OME_ATTRIB_TYPE_INDEX,
    OME_ATTRIB_TYPE_USER_0,
    OME_ATTRIB_TYPE_USER_1,
    OME_ATTRIB_TYPE_USER_2,
    OME_ATTRIB_TYPE_USER_3,
    OME_ATTRIB_MAX
} omeAttribType;


typedef enum omePolygonType
{
    OME_POINTS,
    OME_LINE_STRIP,
    OME_LINE_LOOP,
    OME_LINES,
    OME_LINE_STRIP_ADJACENCY,
    OME_LINES_ADJACENCY,
    OME_TRIANGLE_STRIP,
    OME_TRIANGLE_FAN,
    OME_TRIANGLES,
    OME_TRIANGLE_STRIP_ADJACENCY,
    OME_TRIANGLES_ADJACENCY,
    OME_PATCHES
} omePolygonType;


//TODO: should be private. Internal use only
typedef struct omeVertexAttrib
{
    int nbElements;
    int size;
    void *data;
    int updateHint;
    omeType type;
    omeAttribType geometryType;
    omeBool actived;
    omeBool enabled;
    int loc;
    const char *name;
    const char *glslType;
} omeVertexAttrib;


typedef struct omeGeometry
{
    int nbVertices;
    int nbAttributes;
    int vertexSize;
    int indexCpt;
    int size;
    omeVertexAttrib attributes[OME_ATTRIB_MAX];  //TODO: try dynamic solution instead?
    omeVertexAttrib indices;
    omeBool interleaved;
    omeBool indexed;
    omeBool padded;
    omeBool finalized;
    omePolygonType polygonType;
    unsigned int VBO;
    omeBool VBOReady;

    // TODO: maybe those things should be in a reference struct, used in a "polymorphic" way...
    omeMeshList *references;
    int nbReferences;
} omeGeometry;


// TODO: use a macro for generating the typedef and the struct OME_TYPEDEF_LIST(type) OME_LIST(type)?
typedef struct omeGeometryListElement omeGeometryListElement, omeGeometryList;
struct omeGeometryListElement
{
    omeGeometry *geometry;
    omeGeometryListElement *next;
    omeGeometryListElement *prev;
};


omeGeometry *omeGeometryCreate(int nbVertices, int nbAttributes, omePolygonType polygonType);
void omeGeometryDestroy(omeGeometry **g);
int omeGeometryAddAttrib(omeGeometry *g, int nbElements, omeType type, int updateHint, omeAttribType geometryType, void *data);
int omeGeometryAddIndices(omeGeometry *g, omeType type, int updateHint, void *data);
void omeGeometryFinalize(omeGeometry *g);
void omeGeometryUpdateAttrib(omeGeometry *g, int attribIndex, void *data);
void omeGeometryBuildVBO(omeGeometry *g);
void omeGeometryEnableAttributes(omeGeometry *g, omeProgram *p);
void omeGeometrySendAttributes(omeGeometry *g);
void omeGeometryDisableAttributes(omeGeometry *g);
void omeGeometryRender(omeGeometry *g);
void omeGeometryUseIndices(omeGeometry *g);

// TODO: => omeResourceAddRef() and omeResourceDelRef()???
void omeGeometryAddRef(omeGeometry *g, omeMesh *m);
void omeGeometryDelRef(omeGeometry *g, omeMesh *m);


#ifdef __cplusplus
}
#endif


#endif // OME_GEOMETRY_H
