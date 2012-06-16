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


int main(void)
{
    omeBuffer *buffer;
    omeCamera *camera;
    omeVector pos = {2.f, 2.f, 2.f};
    int positions[] = {
        -1, -1, -1,
        1, -1, -1,
        1, 1, -1,
        1, 1, -1,
        -1, 1, -1,
        -1, -1, -1,

        -1, -1, 0,
        1, -1, 0,
        1, 1, 0,
        1, 1, 0,
        -1, 1, 0,
        -1, -1, 0,

        -1, -1, 1,
        1, -1, 1,
        1, 1, 1,
        1, 1, 1,
        -1, 1, 1,
        -1, -1, 1};

    // get OpenGL context
    if(!glfwInit() || !glfwOpenWindow(640, 480, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
        return EXIT_FAILURE;

    omeEngineStart();

    // camera settings
    camera = omeCameraCreate(OME_CAMERA_TYPE_PERPECTIVE);
    omeCameraSetPerspective(camera, 65.f, 640.f / 480.f, 0.01f, 100.f);
    omeCameraSetPosition(camera, &pos);
    omeCameraUpdate(camera);

    // buffer test
    buffer = omeBufferCreate(54, 1);
    omeBufferAddAttrib(buffer, 3, OME_INT, 0, OME_BUFFER_TYPE_POSITION, positions);
    
    while(glfwGetWindowParam(GLFW_OPENED) && !glfwGetKey(GLFW_KEY_ESC))
    {
        int i;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glRotated(1, 0, 0, 1);
        // TODO: fix the omeMatrixRotateAngles() function
        //glGetFloatv(GL_MODELVIEW_MATRIX, matrix.tab);
        //omeMatrixRotateAngles(&matrix, &vec);
        //omeMatrixLoad(&matrix, OME_TRUE);

        glBegin(GL_TRIANGLES);

        for(i = 0; i < 54; i += 3)
        {
            glColor3f((positions[i] + 1) * 0.5f, (positions[i + 1] + 1) * 0.5f, (positions[i + 2] + 1) * 0.5f);
            glVertex3i(positions[i], positions[i + 1], positions[i + 2]);
        }

        glEnd();

        // TODO: limit fps here?
        omeEngineUpdate();
        glfwSleep(0.001);
        glfwSwapBuffers();
    }

    omeCameraDestroy(&camera);
    omeEngineStop();

    return EXIT_SUCCESS;
}
