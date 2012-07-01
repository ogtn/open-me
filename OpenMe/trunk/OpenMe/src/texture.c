//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "texture.h"
#include "logger.h"
#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>


omeTexture *omeTextureCreate(omeTextureType type, int width, int height, int depth, void *data)
{
    GLenum format = GL_RGBA;
    omeTexture *t = calloc(1, sizeof(omeTexture));

    glGenTextures(1, &t->id);
    t->width = width;
    t->height = height;
    t->depth = depth;
    t->type = omeTextureGetGLType(type);

    omeTextureBind(t);

    // sending pixels to OpenGL
    switch(type)
    {
    case OME_TEXTURE_TYPE_1D:
        omeLoggerLog("texture 1D not supported yet\n");
        return NULL;
        //glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        break;
    case OME_TEXTURE_TYPE_2D:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        break;
    case OME_TEXTURE_TYPE_3D:
        omeLoggerLog("texture 3D not supported yet\n");
        return NULL;
        //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, format, GL_UNSIGNED_BYTE, data);
        break;
    case OME_TEXTURE_TYPE_CUBE:
        omeLoggerLog("Cubemap not supported yet\n");
        return NULL;
        break;
    default:
        omeLoggerLog("invalid type of texture\n");
        return NULL;
        break;
    }

    // set minifying and magnifying filters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // TODO: cubemap
    // TODO: mipmap
    // TODO: extra parameter for hints (mipmaps, filters, internal & external format...) If NULL, default values used...
    // TODO: lot of things to check here: http://www.opengl.org/wiki/Common_Mistakes
    return t;
}


void omeTextureDestroy(omeTexture **t)
{
    glDeleteTextures(1, &(*t)->id);
    memset(*t, 0, sizeof(omeTexture));
    free(*t);
    *t = NULL;
}


void omeTextureBind(omeTexture *t)
{
    glBindTexture(t->type, t->id);
}


unsigned int omeTextureGetGLType(omeTextureType type)
{
    switch(type)
    {
    case OME_TEXTURE_TYPE_1D:   return GL_TEXTURE_1D;
    case OME_TEXTURE_TYPE_2D:   return GL_TEXTURE_2D;
    case OME_TEXTURE_TYPE_3D:   return GL_TEXTURE_3D;
    case OME_TEXTURE_TYPE_CUBE: return GL_TEXTURE_CUBE_MAP;
    default:                    return 0;
    }
}
