//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_CAMERA_H
#define OME_CAMERA_H


#ifdef __cplusplus
extern "C" {
#endif


#include "matrix.h"
#include "utils.h"


// All the different types of camera propose by OpenMe
typedef enum omeCameraType
{
    OME_CAMERA_TYPE_ORTHO,
    OME_CAMERA_TYPE_PERPECTIVE
} omeCameraType;


typedef struct omeCamera
{
    omeMatrix projection;
    omeBool projectionUpToDate;
    omeMatrix view;
    omeBool viewUpToDate;
    omeCameraType type;
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
void omeCameraSetRatio(omeCamera *c, float ratio);
void omeCameraUpdate(omeCamera *c);


#ifdef __cplusplus
}
#endif


#endif // OME_CAMERA_H
