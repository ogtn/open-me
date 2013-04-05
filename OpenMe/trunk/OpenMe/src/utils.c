//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "utils.h"
#include "opengl.h"
#include <IL/il.h>
#include <ctype.h>
#include <string.h>

// gettimeofday() and equivalent...
#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif


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

    #ifdef DEBUG
    default:
        return OME_FAILURE;
    #endif
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
    // case OME_DOUBLE:    return GL_DOUBLE;

    #ifdef DEBUG
    default:
        return (unsigned int)OME_FAILURE;
    #endif
    }
}


const char *omeGetGLError(void)
{
    const char *res;
    GLenum error = glGetError();

    switch(error)
    {
        case GL_NO_ERROR: res = "GL_NO_ERROR"; break;
        case GL_INVALID_ENUM: res = "GL_INVALID_ENUM"; break;
        case GL_INVALID_VALUE: res = "GL_INVALID_VALUE"; break;
        case GL_INVALID_OPERATION: res = "GL_INVALID_OPERATION"; break;
        // case GL_STACK_OVERFLOW: res = "GL_STACK_OVERFLOW"; break;
        // case GL_STACK_UNDERFLOW: res = "GL_STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY: res = "GL_OUT_OF_MEMORY"; break;
        default: res = "unknown error"; break;
    }

    return res;
}


const char *omeGetILError(void)
{
    const char *res;
    ILenum error = ilGetError();

    switch(error)
    {
        case IL_NO_ERROR: 
            res = "No detectable error has occured"; break;
        case IL_INVALID_ENUM:
            res = "An unacceptable enumerated value was passed to a function."; break;
        case IL_OUT_OF_MEMORY:
            res = "Could not allocate enough memory in an operation."; break;
        case IL_FORMAT_NOT_SUPPORTED:
            res = "The format a function tried to use was not able to be used by that function."; break;
        case IL_INTERNAL_ERROR:
            res = "A serious error has occurred. Please e-mail DooMWiz with the conditions leading up to this error being reported."; break;
        case IL_INVALID_VALUE:
            res = "An invalid value was passed to a function or was in a file."; break;
        case IL_ILLEGAL_OPERATION:
            res = "The operation attempted is not allowable in the current state. The function returns with no ill side effects."; break;
        case IL_ILLEGAL_FILE_VALUE:
            res = "An illegal value was found in a file trying to be loaded."; break;
        case IL_INVALID_FILE_HEADER:
            res = "A file's header was incorrect."; break;
        case IL_INVALID_PARAM:
            res = "An invalid parameter was passed to a function, such as a NULL pointer."; break;
        case IL_COULD_NOT_OPEN_FILE:
            res = "Could not open the file specified. The file may already be open by another app or may not exist."; break;
        case IL_INVALID_EXTENSION:
            res = "The extension of the specified filename was not correct for the type of image-loading function."; break;
        case IL_FILE_ALREADY_EXISTS:
            res = "The filename specified already belongs to another file. To overwrite files by default, call ilEnable with the IL_FILE_OVERWRITE parameter."; break;
        case IL_OUT_FORMAT_SAME:
            res = "Tried to convert an image from its format to the same format."; break;
        case IL_STACK_OVERFLOW:
            res = "One of the internal stacks was already filled, and the user tried to add on to the full stack."; break;
        case IL_STACK_UNDERFLOW:
            res = "One of the internal stacks was empty, and the user tried to empty the already empty stack."; break;
        case IL_INVALID_CONVERSION:
            res = "An invalid conversion attempt was tried."; break;
        case IL_LIB_JPEG_ERROR:
            res = "An error occurred in the libjpeg library."; break;
        case IL_LIB_PNG_ERROR:
            res = "An error occurred in the libpng library."; break;
        case IL_UNKNOWN_ERROR:
            res = "No function sets this yet, but it is possible (not probable) it may be used in the future."; break;
        default:
            res = "IL panic: unknow error occured!"; break;
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


void omeCleanString(char *str)
{
    while(*str)
    {
        if(*str == '\n' || *str == '\r')
        {
            *str = '\0';
            return;
        }

        str++;
    }
}


omeBool omeIsBlank(char c)
{
    return c == ' ';
}


void omeSkipBlanks(char **str)
{
    while(**str && omeIsBlank(**str))
        (*str)++;
}


// TODO: maybe replace this thing by a macro which would stand for: 
// omeDbgClearMem(ptr, size), free(ptr) and *ptr = NULL
// In a non debug context, only the free could be kept
// However, the impact on performances is probably negligeable
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

void omeDbgClearMem(void *ptr, size_t size)
{
#ifdef DEBUG
    memset(ptr, 0, size);
#endif
}

#pragma GCC diagnostic pop


double omeGetTime(void)
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
