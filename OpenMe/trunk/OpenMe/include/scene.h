//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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
typedef struct omeMesh omeMesh;
typedef struct omeLight omeLight;


typedef struct omeScene
{
    omeGeometryList *geometries;
    omeLight **lights;
    int nbLights;
} omeScene;


omeScene *omeSceneCreate(void);
void omeSceneDestroy(omeScene **s);
void omeSceneAddGeometry(omeScene *s, omeGeometry *g);
void omeSceneAddLight(omeScene *s, omeLight *l);
void omeSceneRender(omeScene *s, omeCamera *c);
omeMesh *omeScenePick(omeScene *s, omeCamera *c, int x, int y);


#ifdef __cplusplus
}
#endif


#endif // OME_SCENE_H
