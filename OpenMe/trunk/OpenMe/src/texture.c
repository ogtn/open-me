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
#include <IL/il.h>
#include <stdlib.h>
#include <string.h>


static const GLenum omeCubeMapFaces[] = 
{
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z
};


omeTexture *omeTextureCreate(omeTextureType type, int width, int height, int depth, void **data, const char *name)
{
    int i;
    GLenum format = GL_RGBA;
    omeTexture *t = calloc(1, sizeof(omeTexture));

    omeResourceInitialize(&t->resource, OME_RESOURCE_TEXTURE, name);

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
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, width, 0, format, GL_UNSIGNED_BYTE, data[0]);
        break;
    case OME_TEXTURE_TYPE_2D:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data[0]);
        break;
    case OME_TEXTURE_TYPE_3D:
        //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, width, height, depth, 0, format, GL_UNSIGNED_BYTE, data[0]);
        omeLoggerLog("texture 3D not supported yet\n");
        return NULL;
        break;
    case OME_TEXTURE_TYPE_CUBE:
        for(i = 0; i < 6; i++)
            glTexImage2D(omeCubeMapFaces[i], 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data[i]);
        break;
    default:
        omeLoggerLog("invalid type of texture\n");
        return NULL;
        break;
    }

    // set minifying and magnifying filters
    glTexParameterf(t->type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(t->type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // mipmaps
    // TODO: use glGenerateMipmap if GL > 3.0
    glGenerateMipmapEXT(t->type);

    // set wrap modes
    switch(type)
    {
    case OME_TEXTURE_TYPE_3D:
    case OME_TEXTURE_TYPE_CUBE:
        glTexParameteri(t->type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        break;
    case OME_TEXTURE_TYPE_2D:
        glTexParameteri(t->type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    case OME_TEXTURE_TYPE_1D:
        glTexParameteri(t->type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    }

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


omeTexture *omeTextureLoadFromFile(const char *fileName)
{
    ILuint il_id;
    int w, h;
    omeTexture *t;
    void *pixels;

    // texture already loaded
    t = omeResourceFind(fileName);

    if(t != NULL)
    {
        omeResourceAddRef(&t->resource);
        return t;
    }

    // init
    ilInit();
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);

    // create image
    ilGenImages(1, &il_id);
    ilBindImage(il_id);

    if(ilLoadImage(fileName) == IL_FALSE)
    {
        // TODO: remove this
        switch(ilGetError())
        {
        case IL_COULD_NOT_OPEN_FILE: puts("IL_COULD_NOT_OPEN_FILE"); break;
        case IL_ILLEGAL_OPERATION:puts("IL_ILLEGAL_OPERATION"); break;
        case IL_INVALID_EXTENSION: puts("IL_INVALID_EXTENSION"); break;
        case IL_INVALID_PARAM: puts("IL_INVALID_PARAM"); break;
        default: break;
        }

        ilDeleteImage(il_id);
        printf("unable to load: %s\n", fileName);
        return NULL;
    }

    if(ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE) == IL_FALSE)
    {
        ilDeleteImage(il_id);
        printf("Unable to convert: %s\n", fileName);
        return NULL;
    }

    w = ilGetInteger(IL_IMAGE_WIDTH);
    h = ilGetInteger(IL_IMAGE_HEIGHT);
    pixels = ilGetData();

    t = omeTextureCreate2D(w, h, &pixels, fileName);

    ilDeleteImage(il_id);
    ilShutDown();

    return t;
}
