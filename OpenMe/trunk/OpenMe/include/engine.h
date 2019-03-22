//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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
    int totalFrames;            // global frame counter, since omeEngineStart()
    int frames;                 // temporary frame counter, reseted frequently
    double FPS;                 // current FPS, updated frequently
    double FPSUpdateTime;       // TODO: find out what the fuck is this
    double lastFrameDuration;   // TODO: c'mon, why so many useless fields?
    double lastFrameStart;
    double lastFPSCheck;

    // render stuff
    omeRenderTarget *renderTarget;  // cache of the last render target used
    omeCamera *camera;              // active camera
    omeScene *scene;                // active scene
    omeViewport viewport;           // current viewport
    omeProgram *pickingProgram;     // shader program used to process picking
} omeEngine;


//////////////////////// [Public interface] ////////////////////////

// initializes and stop the engine
omeStatus omeEngineStart(int width, int height);
void omeEngineStop(void);

// getters for performance counters
double omeEngineGetFPS(void);
double omeEngineGetFrameDuration(void);

// should be called once per frame, to maintain performance counters up to date
void omeEngineUpdate(void);

// notifie the engine of a window resize, to keep the viewport up to date
void omeEngineResize(int width, int height);

// render the active scene with the active camera
// f no render target is specified (NULL), the render is made on screen
void omeEngineRender(omeRenderTarget *rt);

// set the camera to use for the next render
void omeEngineSetActiveCamera(omeCamera *c);

// set the scene to use for the next render
void omeEngineSetActiveScene(omeScene *s);

// process picking at the given coordinates, with the active scene and camera
// return the mesh picked, or NULL if nothing matches
// TODO: find a way to pick multiple meshes? (ones hiding the others)
omeMesh *omeEnginePick(int x, int y);

//////////////////////// [Private functions] ////////////////////////

// return the active scene
omeScene *omeEngineGetActiveScene(void);

// return the picking program
omeProgram *omeEngineGetPickingProgram(void);

// print GL informations
void omeEngineGLInfo(void);


#ifdef __cplusplus
}
#endif


#endif // OME_ENGINE_H
