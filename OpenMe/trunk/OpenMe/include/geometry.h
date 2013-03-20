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

// List of existing types of vertex attributes
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


// Thoses types exactly match the OpenGL render modes
// TODO: reduce this set in OpenGLES mode
// TODO: rename renderMode
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
// Hold all the information related to a vertex attribute
typedef struct omeVertexAttrib
{
    // data & metadata
    int nbElements;                 // Number of elements
    int size;                       // Size on the device, in bytes
    void *data;                     // Pointer to the data
    omeType type;                   // Type of data elements
    omeAttribType geometryType;     // What kind of attribute is this?
    
    // OpenGL stuff
    int updateHint;                 // Hint used by OpenGL to optimize storage on device
    int loc;                        // Location in the GLSL program
    const char *name;               // Name in the GLSL program
    const char *glslType;           // GLSL type
    
    // internal state    
    omeBool actived;                // Is this attribute valid? TODO: rename this shit "valid"?
    omeBool enabled;                // Is this attribut currently enabled from OpenGL's POV?
} omeVertexAttrib;


// A geometry is a renderabl set of vertices, which are made of a 
// variable number of vertex attributes (see omeVertexAttrib and omeAttribType)
typedef struct omeGeometry
{
    int nbVertices;             // Number of vertices composing the geometry
    int vertexSize;             // Size of each vertex on the device, in bytes
    int nbAttributes;           // Number of attributes 
    omeVertexAttrib attributes[OME_ATTRIB_MAX]; // all the vertex attributes TODO: try dynamic solution instead?
    omeVertexAttrib indices;    // special attribute, use to store the indices of the vertices

    int indexCpt;               // Internal counter TODO: remove this once the vertex attributes will be dynamically stored?
    int size;                   // Total size needed on the the device to store the geometry data TODO: how about the indices??
    
    omeBool interleaved;        // If interleaved, all the attributes (except indices), are interleaved
                                // In the VBO (ababab), instead of being  stored one after the other (aaabbb)
    omeBool indexed;            // For geometry which reuse many times a lot of their vertices, indexing
                                // them can be an interesting optimization
    omeBool padded;             // Padding can improve performance by making vertices size being a
                                // power of two, or an other performance friendly value
    omeBool finalized;          // TODO: this is old stuff, is this still useful?
    omePolygonType polygonType; // Type of polygon stored, triangles, quads... see omePolygonType
    unsigned int VBO;           // OpenGL id of the VBO
    omeBool VBOReady;           // is the VBO generated?

    // TODO: maybe those things should be in a reference struct, used in a "polymorphic" way...
    omeMeshList *references;    // List of meshes using this geometry
    int nbReferences;           // Number of meshes in the list
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
