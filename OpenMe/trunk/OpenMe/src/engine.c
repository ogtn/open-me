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
#include "logger.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glew.h>
#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif


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


void omeEngineClearBuffers(int flags)
{
    glClear(flags);
}


void omeEngineClearAllBuffers(void)
{
    // TODO: call omeEngineClearBuffers() with all the activated buffers
}


int omeEngineStart(void)
{
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
    //glClearColor(0.1f, 0.1f, 0.1f, 1);
    //glEnable(GL_BGR_EXT); // doesn't seem supported on my laptop :/
    //glEnable(GL_ALPHA_TEST);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_TEXTURE_2D); // for later...
    //glAlphaFunc(GL_GREATER, 0.1); // maybe not very usefull...
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // initialize the engine
    engine.FPSUpdateTime = omeEngineGetTime();
    engine.lastFrameStart = omeEngineGetTime();
    engine.state = OME_ENGINE_STATE_STARTED;
    
    return OME_SUCCESS;
}


void omeEngineStop(void)
{
    omeLoggerLog("Engine stopped, status = %s\n", engine.state == OME_ENGINE_STATE_DEFECT ? "KO" : "OK");
    omeLoggerStop();
    engine.state = OME_ENGINE_STATE_STOPPED;
}
