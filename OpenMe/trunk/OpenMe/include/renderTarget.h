//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_RENDER_TARGET_H
#define OME_RENDER_TARGET_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"


// forward declarations
typedef struct omeTexture omeTexture;


typedef struct omeRenderTarget
{
    int width;
    int height;
    unsigned int FBO;
    unsigned int depthBuffer;
    omeTexture *colorBuffer;
    omeBool binded;
} omeRenderTarget;


omeRenderTarget *omeRenderTargetCreate(int width, int height);
void omeRenderTargetDestroy(omeRenderTarget **rt);
void omeRenderTargetBind(omeRenderTarget *rt);
void omeRenderTargetUnbind(void);
void omeRenderTargetUpdateMipMaps(omeRenderTarget *rt);


#ifdef __cplusplus
}
#endif


#endif // OME_RENDER_TARGET_H
