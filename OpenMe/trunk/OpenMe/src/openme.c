//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "openme.h"
#include <stdio.h>
#include <GL/glfw.h>


int ome_Init(int width, int height)
{
    if(!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return OME_FAILURE;
    }

    if(!glfwOpenWindow(width, height, 8, 8, 8, 8, 0, 0, GLFW_WINDOW))
    {
        fprintf(stderr, "Failed to open an OpenGL window\n");
        return OME_FAILURE;
    }

    fprintf(stdout, "OpenGL %d.%d context ready\n", 
        glfwGetWindowParam(GLFW_OPENGL_VERSION_MAJOR),
        glfwGetWindowParam(GLFW_OPENGL_VERSION_MINOR));
        
    return OME_SUCCESS;
}


void ome_Quit(void)
{
    glfwCloseWindow();
    glfwTerminate();
}


void ome_SwapBuffer(void)
{
    glfwSwapBuffers();
}


int ome_isWindowOpened(void)
{
    return glfwGetWindowParam(GLFW_OPENED) ? OME_TRUE : OME_FALSE;
}
