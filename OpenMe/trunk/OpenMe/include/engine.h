//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_ENGINE_H
#define OME_ENGINE_H


#ifdef __cplusplus
extern "C" {
#endif


// forward declaration
typedef struct omeCamera omeCamera;
typedef struct omeGeometry omeGeometry;
typedef struct omeGeometryListElement omeGeometryListElement, omeGeometryList;


typedef enum omeEngineState
{
    OME_ENGINE_STATE_STOPPED,
    OME_ENGINE_STATE_STARTED,
    OME_ENGINE_STATE_DEFECT,
    OME_ENGINE_STATE_MAX
} omeEngineState;


typedef struct omeScene
{
    omeGeometryList *geometries;
} omeScene;


typedef struct omeEngine
{
    omeEngineState state;

    // performances counters
    int totalFrames;
    int frames;
    double FPS;
    double FPSUpdateTime;
    double lastFrameDuration;
    double lastFrameStart;
    double lastFPSCheck;
} omeEngine;


int omeEngineStart(void);
void omeEngineStop(void);

double omeEngineGetTime(void);
double omeEngineGetFPS(void);
double omeEngineGetFrameDuration(void);
void omeEngineUpdate(void);
void omeEngineClearBuffers(int flags);
void omeEngineClearAllBuffers(void);

omeScene *omeSceneCreate(void);
void omeSceneDestroy(omeScene **s);
void omeSceneAddGeometry(omeScene *s, omeGeometry *g);
void omeSceneRender(omeScene *s, omeCamera *c);


#ifdef __cplusplus
}
#endif


#endif // OME_ENGINE_H
