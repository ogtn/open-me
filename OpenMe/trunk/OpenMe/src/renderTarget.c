//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "renderTarget.h"
#include "texture.h"
#include "logger.h"
#include <GL/glew.h>
#include <stdlib.h>
#include <string.h>


// TODO: check for extension before using this...
omeRenderTarget *omeRenderTargetCreate(int width, int height)
{
    omeRenderTarget *rt;
    GLenum status;
    void * null = NULL;

    rt = calloc(1, sizeof(omeRenderTarget));
    rt->width = width;
    rt->height = height;

    // Framebuffer object
    glGenFramebuffersEXT(1, &rt->FBO);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, rt->FBO);
    rt->binded = OME_TRUE;

    // create and allocate color buffer
    rt->colorBuffer = omeTextureCreate2D(width, height, &null, NULL);

    // create and allocate depth buffer
    glGenRenderbuffersEXT(1, &rt->depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, rt->depthBuffer);
    glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);

    // attach buffers to the framebuffer object
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rt->depthBuffer);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rt->colorBuffer->id, 0);

    // check status
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);

    if(status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        omeLoggerLog("Something went wrong with the render target\n");
        omeRenderTargetDestroy(&rt);

        return NULL;
    }

    omeRenderTargetUnbind();

    return rt;
}


void omeRenderTargetDestroy(omeRenderTarget **rt)
{
    if((*rt)->binded)
        omeRenderTargetUnbind();

    glDeleteFramebuffersEXT(1, &(*rt)->FBO);
    glDeleteRenderbuffersEXT(1, &(*rt)->depthBuffer);
    omeResourceDelRef(&(*rt)->colorBuffer->resource);
    omeDbgClearMem(*rt, sizeof(omeRenderTarget));
    free(*rt);
    *rt = NULL;
}


void omeRenderTargetBind(omeRenderTarget *rt)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, rt->FBO);
}


void omeRenderTargetUnbind(void)
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


void omeRenderTargetUpdateMipMaps(omeRenderTarget *rt)
{
    // TODO: omeTextureGenMipMaps() ?  GL_TEXTURE_2D hardcoded is a bad idea
    omeTextureBind(rt->colorBuffer);
    glGenerateMipmapEXT(GL_TEXTURE_2D);
}
