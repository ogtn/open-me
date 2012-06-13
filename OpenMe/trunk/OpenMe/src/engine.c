//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "engine.h"
#include "constants.h"
#include "logger.h"
#include <GL\glfw.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static omeEngine engine;


double omeEngineGetTime(void)
{
    return glfwGetTime();
}


double omeEngineGetFPS(void)
{
    return engine.FPS;
}


double omeEngineGetFrameDuration(void)
{
    return engine.lastFrameDuration;
}


void omeEngineSwapBuffer(void)
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

    // TODO: limit fps here?

    // buffers swapping
    glfwSwapBuffers();
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
    int major, minor, rev;

    // avoid double start
    if(engine.state == OME_ENGINE_STATE_STARTED)
    {
        omeLoggerLog("omeEngineStart() failed: engine already started!\n");
        return OME_FAILURE;
    }

    // clear everything in case of restart
    memset(&engine, 0, sizeof(omeEngine));
    omeLoggerStart();

    // start glfw
    if(!glfwInit())
    {
        engine.state = OME_ENGINE_STATE_DEFECT;
        omeLoggerLog("omeEngineStart() failed: failed to initialize GLFW\n");
        return OME_FAILURE;
    }

    // initialize the engine
    engine.FPSUpdateTime = omeEngineGetTime();
    engine.lastFrameStart = omeEngineGetTime();
    engine.state = OME_ENGINE_STATE_STARTED;
    
    // a few prints
    omeLoggerLog("Engine successfully started\n");
    omeLoggerLog("%d processors available\n", glfwGetNumberOfProcessors());
    glfwGetVersion(&major, &minor, &rev);
    omeLoggerLog("GLFW version: %d.%d.%d\n", major, minor, rev);
    glfwGetGLVersion(&major, &minor, &rev);
    omeLoggerLog("OpenGL version: %d.%d.%d\n", major, minor, rev);
    // TODO: glfwGetVideoModes() and glfwGetDesktopMode()

    return OME_SUCCESS;
}


int omeEngineOpenWindow(int width, int height, int fullScreen)
{
    // TODO: allow user to select the bits for each buffer, through another function maybe
    if(!glfwOpenWindow(width, height, 8, 8, 8, 8, 0, 0, fullScreen ? GLFW_FULLSCREEN : GLFW_WINDOW))
    {
        omeLoggerLog("Failed to open an OpenGL window\n");
        return OME_FAILURE;
    }

    omeLoggerLog("Window successfully opened");
    omeLoggerLog("OpenGL %d.%d context ready\n", 
        glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR),
        glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR));

    return OME_SUCCESS;
}


int omeEngineIsWindowOpened(void)
{
    return glfwGetWindowParam(GLFW_OPENED) ? OME_TRUE : OME_FALSE;
}


void omeEngineStop(void)
{
    glfwCloseWindow();
    glfwTerminate();

    omeLoggerLog("Engine stopped, status = %s\n", engine.state == OME_ENGINE_STATE_DEFECT ? "KO" : "OK");
    omeLoggerStop();
    engine.state = OME_ENGINE_STATE_STOPPED;
}
