//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_SCENE_H
#define OME_SCENE_H


#ifdef __cplusplus
extern "C" {
#endif


// forward declaration
typedef struct omeCamera omeCamera;
typedef struct omeGeometry omeGeometry;
typedef struct omeGeometryListElement omeGeometryListElement, omeGeometryList;


typedef struct omeScene
{
    omeGeometryList *geometries;
} omeScene;


omeScene *omeSceneCreate(void);
void omeSceneDestroy(omeScene **s);
void omeSceneAddGeometry(omeScene *s, omeGeometry *g);
void omeSceneRender(omeScene *s, omeCamera *c);


#ifdef __cplusplus
}
#endif


#endif // OME_SCENE_H
