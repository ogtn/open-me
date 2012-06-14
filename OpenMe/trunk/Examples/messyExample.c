//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include <openme.h>
#include <stdlib.h>
#include <stdio.h>


// TODO: useful for debugging, put that one in something like utils.c
void printError(void)
{
    GLenum error = glGetError();

    switch(error)
    {
    case GL_NO_ERROR: puts("GL_NO_ERROR"); break;
    case GL_INVALID_ENUM: puts("GL_INVALID_ENUM"); break;
    case GL_INVALID_VALUE: puts("GL_INVALID_VALUE"); break;
    case GL_INVALID_OPERATION: puts("GL_INVALID_OPERATION"); break;
    case GL_STACK_OVERFLOW: puts("GL_STACK_OVERFLOW"); break;
    case GL_STACK_UNDERFLOW: puts("GL_STACK_UNDERFLOW"); break;
    case GL_OUT_OF_MEMORY: puts("GL_OUT_OF_MEMORY"); break;
    default: puts("unknown error :("); break;
    }
}


int main(void)
{
    double startTime, totalTime;
    int frames = 0;
    omeVector pos =     {2.f, 2.f, 2.f};
    omeVector target =  {0.f, 0.f, 0.f};
    omeVector up =      {0.f, 0.f, 1.f};
    omeCamera *camera = omeCameraAlloc(OME_CAMERA_TYPE_PERPECTIVE);

    // opening window
    omeEngineStart();
    omeEngineOpenWindow(640, 480, 0);

    // clear color and transparency
    //glClearColor(0.1f, 0.1f, 0.1f, 1);
    //glEnable(GL_BGR_EXT); // doesn't seem supported on my laptop :/
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_TEXTURE_2D); // for later...
    //glAlphaFunc(GL_GREATER, 0.1); // maybe not very usefull...
    //glEnable(GL_DEPTH_TEST); // wtf?? break transarency?

    // projection matrix settings
    omeCameraSetPerspective(camera, 65.f, 640.f / 480.f, 0.01f, 100.f);
    omeCameraSetLookAt(camera, &pos, &target, &up);
    omeCameraUpdate(camera);

    startTime = glfwGetTime();

    while(omeEngineIsWindowOpened() && !glfwGetKey(GLFW_KEY_ESC))
    {
        omeVector vec = {0, 0, 1};

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glRotated(1, 0, 0, 1);
        // TODO: fix the omeMatrixRotateAngles() function
        //glGetFloatv(GL_MODELVIEW_MATRIX, matrix.tab);
        //omeMatrixRotateAngles(&matrix, &vec);
        //omeMatrixLoad(&matrix, OME_TRUE);

        glBegin(GL_QUADS);

        // bottom plan
        glColor4f(1, 0, 0, 0.5f);
        glVertex3i(-1, -1, -1);
        glVertex3i(1, -1, -1);
        glVertex3i(1, 1, -1);
        glVertex3i(-1, 1, -1);

        // middle plan
        glColor4f(0, 1, 0, 0.5f);
        glVertex2i(-1, -1);
        glVertex2i(1, -1);
        glVertex2i(1, 1);
        glVertex2i(-1, 1);

        // top plan
        glColor4f(0, 0, 1, 0.5f);
        glVertex3i(-1, -1, 1);
        glVertex3i(1, -1, 1);
        glVertex3i(1, 1, 1);
        glVertex3i(-1, 1, 1);

        glEnd();

        frames++;

        // TODO: get rid of that stupid thing, which shouldn't be part of the engine
        omeEngineSwapBuffer();
    }

    totalTime = glfwGetTime() - startTime;
    printf("%d frames rendered in %.3fs\n", frames, totalTime);
    printf("average frame: %fs (%.2f FPS)\n", totalTime / frames, frames / totalTime);
    omeEngineStop();

    return EXIT_SUCCESS;
}
