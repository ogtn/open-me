//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "opengl.h"
#include "logger.h"


omeStatus omeOpenGLInit(void)
{
	#ifndef OME_USE_GLES
	if(glewInit() != GLEW_OK)
    {
        omeLoggerLog("Failed to load glew, you're screwed\n\n");
        return OME_FAILURE;
    }
    #endif

    return OME_SUCCESS;
}
