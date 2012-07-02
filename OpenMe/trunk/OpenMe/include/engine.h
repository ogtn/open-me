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


#include "utils.h"


// forward declaration
typedef struct omeCamera omeCamera;
typedef struct omeScene omeScene;
typedef struct omeRenderTarget omeRenderTarget;
typedef struct omeProgram omeProgram;
typedef struct omeMesh omeMesh;


typedef enum omeEngineState
{
    OME_ENGINE_STATE_STOPPED,
    OME_ENGINE_STATE_STARTED,
    OME_ENGINE_STATE_DEFECT,
    OME_ENGINE_STATE_MAX
} omeEngineState;


typedef struct omeViewport
{
    int x;
    int y;
    int width;
    int height;
    omeBool upToDate;
} omeViewport;


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

    // render stuff
    omeRenderTarget *renderTarget;
    omeCamera *camera;
    omeScene *scene;
    omeViewport viewport;
    omeProgram *pickingProgram;
} omeEngine;


int omeEngineStart(int width, int height);
void omeEngineStop(void);

double omeEngineGetTime(void);
double omeEngineGetFPS(void);
double omeEngineGetFrameDuration(void);
void omeEngineUpdate(void);
void omeEngineResize(int width, int height);

void omeEngineRender(omeRenderTarget *rt);
void omeEngineSetActiveCamera(omeCamera *c);
void omeEngineSetActiveScene(omeScene *s);
omeScene *omeEngineGetActiveScene(void);
omeProgram *omeEngineGetPickingProgram(void);
omeMesh *omeEnginePick(int x, int y);


#ifdef __cplusplus
}
#endif


#endif // OME_ENGINE_H
