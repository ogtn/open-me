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
#include <utlist.h>

// gettimeofday() and equivalent...
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
    glClearColor(0.8f, 0.8f, 0.8f, 1);
    //glEnable(GL_BGR_EXT); // doesn't seem supported on my laptop :/
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glAlphaFunc(GL_GREATER, 0.1); // maybe not very usefull...
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // initialize the engine
    engine.FPSUpdateTime = omeEngineGetTime();
    engine.lastFrameStart = omeEngineGetTime();
    engine.state = OME_ENGINE_STATE_STARTED;

    omeLoggerLog("OpenGL %s\n", glGetString(GL_VERSION));
    omeLoggerLog("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    return OME_SUCCESS;
}


void omeEngineStop(void)
{
    omeLoggerLog("Engine stopped, status = %s\n", engine.state == OME_ENGINE_STATE_DEFECT ? "KO" : "OK");
    omeLoggerStop();
    engine.state = OME_ENGINE_STATE_STOPPED;
}


#include "camera.h"
#include "geometry.h"
#include "mesh.h"
#include "shaderProgram.h"


omeScene *omeSceneCreate(void)
{
    omeScene *s = calloc(1, sizeof(omeScene));

    return s;
}


void omeSceneDestroy(omeScene **s)
{

    memset(*s, 0, sizeof(omeScene));
    free(*s);
    *s = NULL;
}


void omeSceneAddGeometry(omeScene *s, omeGeometry *g)
{
    omeGeometryListElement *element = calloc(1, sizeof(omeGeometryListElement));
    element->geometry = g;

    DL_APPEND(s->geometries, element);
}


void omeSceneRender(omeScene *s, omeCamera *c)
{
    omeGeometryListElement *geometryElt;
    
    DL_FOREACH(s->geometries, geometryElt)
    {
        omeMeshListElement *meshElt;
        omeGeometry *g = geometryElt->geometry;

        omeGeometrySendAttributes(g);

        DL_FOREACH(g->references, meshElt)
        {         
            omeProgram *p = meshElt->mesh->program;
            omeMaterial *m = meshElt->mesh->material;

            // TODO: When the code will be cleaner, remove this. This engine is meant to be shader only
            if(p)
            {
                omeProgramUse(p);

                // TODO: same here I guess... (default material set in meshCreate() maybe?)
                if(m)
                    omeProgramSendUniformMaterial(p, m, "mat");
            }

            omeGeometryRenderVBO(g);
        }

        omeGeometryDisableAttributes(g);
    }
}
