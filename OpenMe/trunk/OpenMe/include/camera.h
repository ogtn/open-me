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
    // internal state
    float near;                 // near clipping plane
    float far;                  // far clipping plane
    
    union {
        // camera ortho, for 2D views
        struct
        {
            float left;         // left clipping plane
            float right;        // right clipping plane
            float bottom;       // bottom clipping plane
            float top;          // top clipping plane
        };

        // camera perspective
        struct
        {
            omeVector pos;      // position in space
            omeVector target;   // direction of the view
            omeVector up;       // up vector: where the fuck is the sky?
            float fov;          // horizontal field of view
            float ratio;        // horizontal/vertcal ratio
        };
    };

    // thoses matrices are meant to be sent to the shaders on each frame
    // they hold the above fields, which are more human readable
    omeMatrix projection;       // projection matrix
    omeBool projectionUpToDate; // state of the projection matrix
    omeMatrix view;             // view matrix
    omeBool viewUpToDate;       // state of the view matrix
    omeCameraType type;         // type of the camera
} omeCamera;


// Constructor and destructor
omeCamera *omeCameraCreate(omeCameraType type);
void omeCameraDestroy(omeCamera **c);

// setters of the different properties
// type check is performed
// those functions mark the camera as out of date, so a call to omeCameraUpdate() is needed before further use
void omeCameraSetOrtho(omeCamera *c, float left, float right, float bottom, float top, float near, float far);
void omeCameraSetPerspective(omeCamera *c, float fov, float ratio, float near, float far);
void omeCameraSetLookAt(omeCamera *c, omeVector *pos, omeVector *target, omeVector *up);
void omeCameraSetPosition(omeCamera *c, omeVector *pos);
void omeCameraSetTarget(omeCamera *c, omeVector *target);
void omeCameraSetRatio(omeCamera *c, float ratio);

// Update the internal matrices
void omeCameraUpdate(omeCamera *c);


#ifdef __cplusplus
}
#endif


#endif // OME_CAMERA_H
