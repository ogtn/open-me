//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "logger.h"
#include "camera.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>



static const omeVector defaultPosition =	{{1, 1, 1}};
static const omeVector defaultTarget = 		{{0, 0, 0}};
static const omeVector defaultUp = 			{{0, 0, 1}};


omeCamera *omeCameraCreate(omeCameraType type)
{
    omeCamera *c = calloc(1, sizeof(omeCamera));

    c->type = type;
    c->state = OME_CAMERA_STATE_OUTDATED;
    omeVectorCopy(&c->pos, &defaultPosition);
    omeVectorCopy(&c->target, &defaultTarget);
    omeVectorCopy(&c->up, &defaultUp);

    return c;
}


void omeCameraDestroy(omeCamera **c)
{
    memset(*c, 0, sizeof(omeCamera));
    free(*c);
    *c = NULL;
}


void omeCameraSetOrtho(omeCamera *c, float left, float right, float bottom, float top, float near, float far)
{
    if(c->type != OME_CAMERA_TYPE_ORTHO)
    {
        omeLoggerLog("this is not an ortho camera");
        return;
    }

    c->state = OME_CAMERA_STATE_OUTDATED;
    c->left = left;
    c-> right = right;
    c-> bottom = bottom;
    c->top = top;
    c->near = near;
    c->far = far;
}


void omeCameraSetPerspective(omeCamera *c, float fov, float ratio, float near, float far)
{
    if(c->type != OME_CAMERA_TYPE_PERPECTIVE)
    {
        omeLoggerLog("This is not a perspective camera");
        return;
    }

    c->state = OME_CAMERA_STATE_OUTDATED;
    c->fov = fov;
    c->ratio = ratio;
    c->near = near;
    c->far = far;
}


void omeCameraSetLookAt(omeCamera *c, omeVector *pos, omeVector *target, omeVector *up)
{
    if(c->type != OME_CAMERA_TYPE_PERPECTIVE)
    {
        omeLoggerLog("this is not a perspective camera");
        return;
    }

    c->state = OME_CAMERA_STATE_OUTDATED;
    omeVectorCopy(&c->pos, pos);
    omeVectorCopy(&c->target, target);
    omeVectorCopy(&c->up, up);
}


void omeCameraSetPosition(omeCamera *c, omeVector *pos)
{
    if(c->type != OME_CAMERA_TYPE_PERPECTIVE)
    {
        omeLoggerLog("this is not a perspective camera");
        return;
    }

    c->state = OME_CAMERA_STATE_OUTDATED;
    omeVectorCopy(&c->pos, pos);
}


void omeCameraSetTarget(omeCamera *c, omeVector *target)
{
    if(c->type != OME_CAMERA_TYPE_PERPECTIVE)
    {
        omeLoggerLog("this is not a perspective camera");
        return;
    }

    c->state = OME_CAMERA_STATE_OUTDATED;
    omeVectorCopy(&c->target, target);
}


void omeCameraSetRatio(omeCamera *c, float ratio)
{
    if(c->type != OME_CAMERA_TYPE_PERPECTIVE)
    {
        omeLoggerLog("this is not a perspective camera");
        return;
    }

    c->state = OME_CAMERA_STATE_OUTDATED;
    c->ratio = ratio;
}


void omeCameraUpdate(omeCamera *c)
{
    // TODO: add a boolean to avoid recomputation
    switch(c->type)
    {
    case OME_CAMERA_TYPE_PERPECTIVE:
        glMatrixMode(GL_PROJECTION);
        omeMatrixMakePerspective(&c->projection, c->fov, c->ratio, c->near, c->far);

        glMatrixMode(GL_MODELVIEW);
        omeMatrixMakeLookAt(&c->modelview, &c->pos, &c->target, &c->up);
        break;
    case OME_CAMERA_TYPE_ORTHO:
        glMatrixMode(GL_PROJECTION);
        omeMatrixMakeOrtho(&c->projection, c->left, c->right, c->bottom, c->top, c->near, c->far);
        break;
    default:
        omeLoggerLog("Unknown type of camera");
        return;
    }

    c->state = OME_CAMERA_STATE_UP_TO_DATE;
}
