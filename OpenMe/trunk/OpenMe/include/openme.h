//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_OPENME_H
#define OME_OPENME_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include "engine.h"
#include "matrix.h"
#include "camera.h"
#include "geometry.h"
#include "mesh.h"
#include "OBJLoader.h"
#include "program.h"
#include "material.h"
#include "scene.h"
#include "texture.h"
#include "renderTarget.h"
#include "light.h"
#include "dstring.h"
#include "pluginLoader.h"


// TODO: test *alloc() return in debug mode, through "overloading"?
// TODO: add an omeBool field in structures, so they can know if they are dynamic ok not and avoid calling free()??? Not sure if stupid....
// TODO: make sure all static stuffs are const, to allow multithreading
// TODO: use glewIsSupported() to adapt the render to the hardware's capabilities

// TODO: get rid of the forward declarations:
// omeCamera
// omeGeometry
// omeGeometryListElement
// omeGeometryList
// omeMesh
// omeMeshListElement
// omeMeshList
// omeLight
// omeTexture
// omePolygonType
// omeProgram
// omeMaterial
// omeScene
// omeRenderTarget
// omeVector
// omeMatrix
//
// in files:
// ../OpenMe/include/scene.h
// ../OpenMe/include/renderTarget.h
// ../OpenMe/include/openme.h
// ../OpenMe/include/mesh.h
// ../OpenMe/include/engine.h
// ../OpenMe/include/OBJLoader.h
// ../OpenMe/include/geometry.h
// ../OpenMe/include/program.h

#ifdef __cplusplus
}
#endif


#endif // OME_OPENME_H
