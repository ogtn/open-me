//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_CAMERA_H
#define OME_CAMERA_H


#include "matrix.h"


typedef enum omeCameraType
{
    OME_CAMERA_TYPE_ORTHO,
    OME_CAMERA_TYPE_PERPECTIVE,
    OME_CAMERA_TYPE_MAX
} omeCameraType;


typedef enum omeCameraState
{
    OME_CAMERA_STATE_OUTDATED,
    OME_CAMERA_STATE_UP_TO_DATE,
    OME_CAMERA_STATE_MAX
} omeCameraState;


typedef struct omeCamera
{
    omeMatrix projection;
    omeMatrix modelview;
    omeCameraType type;
    omeCameraState state;
    float near;
    float far;
    
    union {
        // camera ortho
        struct
        {
            float left;
            float right;
            float bottom;
            float top;
        };

        // camera perspective
        struct
        {
            omeVector pos;
            omeVector target;
            omeVector up;
            float fov;
            float ratio;
        };
    };
} omeCamera;


omeCamera *omeCameraCreate(omeCameraType type);
void omeCameraDestroy(omeCamera **c);
void omeCameraSetOrtho(omeCamera *c, float left, float right, float bottom, float top, float near, float far);
void omeCameraSetPerspective(omeCamera *c, float fov, float ratio, float near, float far);
void omeCameraSetLookAt(omeCamera *c, omeVector *pos, omeVector *target, omeVector *up);
void omeCameraSetPosition(omeCamera *c, omeVector *pos);
void omeCameraSetTarget(omeCamera *c, omeVector *target);
void omeCameraUpdate(omeCamera *c);


#endif // OME_CAMERA_H
