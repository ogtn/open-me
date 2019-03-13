//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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
#include "opengl.h"
#include <stdlib.h>


// TODO: how about multithreading???
static omeEngine engine;


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
    double now = omeGetTime();

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

    omeCameraUpdate(engine.camera);
}


void omeEngineResize(int width, int height)
{
    omeViewport *v = &engine.viewport;
    
    v->width = width;
    v->height = height;
    v->upToDate = OME_FALSE;
}


void GLAPIENTRY omeDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
    const char *str_source;
    const char *str_type;
    const char *str_severity;

    switch(source)
    {
    case GL_DEBUG_SOURCE_API_ARB:               str_source = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:     str_source = "Window system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:   str_source = "Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:       str_source = "Third party"; break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:       str_source = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER_ARB: default:    str_source = "Unkown source"; break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR_ARB:               str_type = "error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: str_type = "deprecated behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  str_type = "undefined behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY_ARB:         str_type = "portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE_ARB:         str_type = "performance"; break;
    case GL_DEBUG_TYPE_OTHER_ARB: default:      str_type = "unknown type"; break;
    }

    switch(severity)
    {
    case GL_DEBUG_SEVERITY_HIGH_ARB:    str_severity = "high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:  str_severity = "medium"; break;
    case GL_DEBUG_SEVERITY_LOW_ARB:     str_severity = "low"; break;
    }

    omeLoggerLog("omeDebugCallback: %s", message);
}


omeStatus omeEngineStart(int width, int height)
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

    if(omeOpenGLInit() == OME_FAILURE)
        return OME_FAILURE;

    // TODO: add a hint to the omeEngineStartParameter to ask for debug, or provide a function to toggle it afterwards
    if(glDebugMessageCallbackARB)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB((GLDEBUGPROCARB)omeDebugCallback, NULL);
    }

    //clear color and transparency
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    //glEnable(GL_BGR_EXT); // doesn't seem supported on my laptop :/
    //glEnable(GL_ALPHA_TEST); // TODO: not GLES2 compliant
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Z buffer
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // initialize the engine
    engine.FPSUpdateTime = omeGetTime();
    engine.lastFrameStart = omeGetTime();
    engine.state = OME_ENGINE_STATE_STARTED;

    // viewport
    v->x = v->y = 0;
    v->width = width;
    v->height = height;
    v->upToDate = OME_FALSE;

    // print info about OpenGL context
    omeLoggerLog("OpenGL %s\n", glGetString(GL_VERSION));
    omeLoggerLog("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &value);
    omeLoggerLog("Max vertex attributes availables: %d\n", value);

    // picking program
    engine.pickingProgram = omeProgramCreate();
    omeProgramAddShader(engine.pickingProgram, omeShaderLoadFromFile("data/picking.vs"));
    omeProgramAddShader(engine.pickingProgram, omeShaderLoadFromFile("data/picking.ps"));
    omeProgramLink(engine.pickingProgram);

    engine.scene = omeSceneCreate();

    return OME_SUCCESS;
}


void omeEngineStop(void)
{
    omeSceneDestroy(&engine.scene);

    omeLoggerLog("Engine stopped, status = %s\n", engine.state == OME_ENGINE_STATE_DEFECT ? "KO" : "OK");
    omeLoggerLog("Last OpenGL error: %s\n", omeGetGLError());
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
