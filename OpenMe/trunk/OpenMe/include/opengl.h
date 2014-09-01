//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_OPENGL_H
#define OME_OPENGL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "utils.h"

#ifdef OME_USE_GLES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <GL/glew.h>
#endif


omeStatus omeOpenGLInit(void);


#ifdef __cplusplus
}
#endif


#endif // OME_OPENGL_H
