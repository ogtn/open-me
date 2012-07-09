//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "utils.h"
#include <GL/glew.h>


int omeSizeOf(omeType type)
{
    switch(type)
    {
    case OME_BYTE:  case OME_UBYTE:
        return 1;
    case OME_SHORT: case OME_USHORT:
        return sizeof(short);
    case OME_INT:   case OME_UINT:
        return sizeof(int);
    case OME_FLOAT:
        return sizeof(float);
    case OME_DOUBLE:
        return sizeof(double);
    default:
        return OME_FAILURE;
    }
}


unsigned int omeTypeToGL(omeType type)
{
    switch (type)
    {
    case OME_BYTE:      return GL_BYTE;
    case OME_UBYTE:     return GL_UNSIGNED_BYTE;
    case OME_SHORT:     return GL_SHORT;
    case OME_USHORT:    return GL_UNSIGNED_SHORT;
    case OME_INT:       return GL_INT;
    case OME_UINT:      return GL_UNSIGNED_INT;
    case OME_FLOAT:     return GL_FLOAT;
    case OME_DOUBLE:    return GL_DOUBLE;
    default:
        return (unsigned int)OME_FAILURE;
    }
}


char *omeGetGLError(void)
{
    GLenum error = glGetError();
    char *res;

    switch(error)
    {
    case GL_NO_ERROR: res = "GL_NO_ERROR"; break;
    case GL_INVALID_ENUM: res = "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE: res = "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION: res = "GL_INVALID_OPERATION"; break;
    case GL_STACK_OVERFLOW: res = "GL_STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW: res = "GL_STACK_UNDERFLOW"; break;
    case GL_OUT_OF_MEMORY: res = "GL_OUT_OF_MEMORY"; break;
    default: res = "unknown error"; break;
    }

    return res;
}


int omeNextPowOfTwo(int n)
{
    int res = 1;

    while(res < n)
        res <<= 1;

    return res;
}
