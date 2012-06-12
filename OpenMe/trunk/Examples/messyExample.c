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
    double startTime, totalTime;
    int frames = 0;

    // opening window
    ome_Init(640, 480);

    // clear color and transparency
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glEnable(GL_BGR_EXT);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_TEXTURE_2D); // for later...
    //glAlphaFunc(GL_GREATER, 0.1); // maybe not very usefull...
    //glEnable(GL_DEPTH_TEST); // wtf?? break transarency?

    // matrix settings
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0f, 640.f/480.f, 0.01f, 100.0f );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // camera settings
    gluLookAt(2.0f, 2.0f, 2.0f,         // Eye-position
                   0.0f, 0.0f, 0.0f,    // View-point
                   0.0f, 0.0f, 1.0f);   // Up-vector
    
    startTime = glfwGetTime();

    while(ome_isWindowOpened())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        glColor4f(1, 1, 1, 0.2f);
        glBegin(GL_QUADS);
        glVertex2i(-1, -1);
        glVertex2i(1, -1);
        glVertex2i(1, 1);
        glVertex2i(-1, 1);
        glEnd();

        frames++;

        ome_SwapBuffer();
    }

    totalTime = glfwGetTime() - startTime;
    printf("%d frames rendered in %.3fs\n", frames, totalTime);
    printf("average frame: %fs (%.2f FPS)\n", totalTime / frames, frames / totalTime);
    ome_Quit();

    return EXIT_SUCCESS;
}
