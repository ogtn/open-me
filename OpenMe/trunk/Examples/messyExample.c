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
#include <GL/glfw.h>


// TODO: useful for debugging, put that one in something like utils.c or error.c?
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
    omeVector pos =     {2.f, 2.f, 2.f};
    omeVector target =  {0.f, 0.f, 0.f};
    omeVector up =      {0.f, 0.f, 1.f};
    omeCamera *camera = omeCameraAlloc(OME_CAMERA_TYPE_PERPECTIVE);

    // initialize glfw
    if(!glfwInit())
        return EXIT_FAILURE;

    // opening window
    if(!glfwOpenWindow(640, 480, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
        return EXIT_FAILURE;

    omeEngineStart();

    // projection matrix settings
    omeCameraSetPerspective(camera, 65.f, 640.f / 480.f, 0.01f, 100.f);
    omeCameraSetLookAt(camera, &pos, &target, &up);
    omeCameraUpdate(camera);

    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
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

        // TODO: limit fps here?
        omeEngineUpdate();
        glfwSwapBuffers();
    }

    omeCameraFree(&camera);
    omeEngineStop();

    return EXIT_SUCCESS;
}
