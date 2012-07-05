//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "engine.h"
#include "utils.h"
#include "scene.h"
#include "logger.h"
#include "renderTarget.h"
#include "camera.h"
#include "mesh.h"
#include "program.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glew.h>

// gettimeofday() and equivalent...
#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif


// TODO: how about multithreading???
static omeEngine engine;


double omeEngineGetTime(void)
{
#ifdef _MSC_VER
    struct _timeb timebuffer;
    _ftime64_s(&timebuffer);
    return timebuffer.time + timebuffer.millitm * 1.0e-3;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1.0e-6;
#endif
}


double omeEngineGetFPS(void)
{
    return engine.FPS;
}


double omeEngineGetFrameDuration(void)
{
    return engine.lastFrameDuration;
}


void omeEngineUpdate(void)
{
    double now = omeEngineGetTime();

    // performaces counters
    engine.totalFrames++;
    engine.frames++;
    engine.lastFrameDuration = now - engine.lastFrameStart;
    engine.lastFrameStart = now;

    // update FPS counter if needed
    if(engine.lastFPSCheck - now >= engine.FPSUpdateTime)
    {
        engine.lastFPSCheck = now;
        engine.FPS = engine.frames / engine.FPSUpdateTime;
        engine.frames = 0;
    }
}


void omeEngineResize(int width, int height)
{
    omeViewport *v = &engine.viewport;
    
    v->width = width;
    v->height = height;
    v->upToDate = OME_FALSE;
}


int omeEngineStart(int width, int height)
{
    int value;
    omeViewport *v = &engine.viewport;

    // avoid double start
    if(engine.state == OME_ENGINE_STATE_STARTED)
    {
        omeLoggerLog("omeEngineStart() failed: engine already started!\n");
        return OME_FAILURE;
    }

    // clear everything in case of restart
    memset(&engine, 0, sizeof(omeEngine));
    omeLoggerStart();

    if(glewInit() != GLEW_OK)
    {
        omeLoggerLog("Failed to load glew, you're screwed\n\n");
        return OME_FAILURE;
    }

    //clear color and transparency
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    //glEnable(GL_BGR_EXT); // doesn't seem supported on my laptop :/
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Z buffer
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // initialize the engine
    engine.FPSUpdateTime = omeEngineGetTime();
    engine.lastFrameStart = omeEngineGetTime();
    engine.state = OME_ENGINE_STATE_STARTED;

    // viewport
    v->x = v->y = 0;
    v->width = width;
    v->height = height;
    v->upToDate = OME_FALSE;

    // picking program
    engine.pickingProgram = omeProgramCreate();
    omeProgramAddShader(engine.pickingProgram, omeShaderLoadFromFile("data/picking.vs"));
    omeProgramAddShader(engine.pickingProgram, omeShaderLoadFromFile("data/picking.ps"));
    omeProgramLink(engine.pickingProgram);

    omeLoggerLog("OpenGL %s\n", glGetString(GL_VERSION));
    omeLoggerLog("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);
    omeLoggerLog("Max vertex attributes availables: %d\n", value);

    engine.scene = omeSceneCreate();

    return OME_SUCCESS;
}


void omeEngineStop(void)
{
    omeSceneDestroy(&engine.scene);

    omeLoggerLog("Engine stopped, status = %s\n", engine.state == OME_ENGINE_STATE_DEFECT ? "KO" : "OK");
    omeLoggerLog("%d frames rendered\n", engine.totalFrames);
    
    omeLoggerStop();
    engine.state = OME_ENGINE_STATE_STOPPED;
}


void omeEngineRender(omeRenderTarget *rt)
{
    omeViewport *v = &engine.viewport;

    if(rt != engine.renderTarget || v->upToDate == OME_FALSE)
    {
        engine.renderTarget = rt;
        v->upToDate = OME_TRUE;

        // render to texture
        if(rt != NULL)
        {
            glClearColor(0, 0, 0, 1);
            omeRenderTargetBind(rt);
            glViewport(0, 0, rt->width, rt->height);
            omeCameraSetRatio(engine.camera, (float)rt->width / rt->height);
        }
        // default buffer
        else
        {
            glClearColor(0.2f, 0.2f, 0.2f, 1);
            omeRenderTargetUnbind();
            glViewport(0, 0, v->width, v->height);
            omeCameraSetRatio(engine.camera, (float)v->width / v->height);
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    omeSceneRender(engine.scene, engine.camera);

    if(rt != NULL)
        omeRenderTargetUpdateMipMaps(rt);
}


// TODO: add a way to set the origin
omeMesh *omeEnginePick(int x, int y)
{
    omeViewport *v = &engine.viewport;

    // avoiding out of viewport picking
    if(x < engine.viewport.x || y < engine.viewport.y
        || x > engine.viewport.width || y > engine.viewport.height)
    {
        omeLoggerLog("No picking allowed in this area: %d %d\n", x, y);
        return NULL;
    }

    engine.viewport.upToDate = OME_FALSE;

    glClearColor(1, 1, 1, 1);
    omeRenderTargetUnbind();
    glViewport(0, 0, v->width, v->height);
    omeCameraSetRatio(engine.camera, (float)v->width / v->height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return omeScenePick(engine.scene, engine.camera, x, y);
}


void omeEngineSetActiveCamera(omeCamera *c)
{
    engine.camera = c;
}


void omeEngineSetActiveScene(omeScene *s)
{
    engine.scene = s;
}


omeScene *omeEngineGetActiveScene(void)
{
    return engine.scene;
}


omeProgram *omeEngineGetPickingProgram(void)
{
    return engine.pickingProgram;
}
