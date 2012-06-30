//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.



#ifndef OME_MATRIX_H
#define OME_MATRIX_H


#ifdef __cplusplus
extern "C" {
#endif


#include "vector.h"


// TODO: use GLfloat instead of float?
// TODO: use sed to generate the double versions for omeMatrix and omeVector
// or a macro version of thoses files, through something like DECLARE_MATRIX(double, d)
typedef union omeMatrix
{
    float data[4][4];
    float tab[16];
} omeMatrix;


void omeMatrixCopy(omeMatrix *dst, const omeMatrix *src);
void omeMatrixMakeIdentity(omeMatrix *m);
void omeMatrixMakeOrtho(omeMatrix *m, float left, float right, float bottom, float top, float zNear, float zFar);
void omeMatrixMakePerspective(omeMatrix *m, float angle, float ratio, float near, float far);
void omeMatrixMakeLookAt(omeMatrix *m, const omeVector *pos, const omeVector *target, const omeVector *up);
void omeMatrixTranslate(omeMatrix *m, const omeVector *v);
void omeMatrixScale(omeMatrix *m, const omeVector *v);
void omeMatrixRotateAxis(omeMatrix *m, const omeVector *axis, float theta);
void omeMatrixRotateAngles(omeMatrix *m, const omeVector *v);
void omeMatrixTranspose(omeMatrix *m);
void omeMatrixLoad(const omeMatrix *m, int transpose);
void omeMatrixMultMatrix(const omeMatrix *m, const omeMatrix *m2, omeMatrix *res);
//void omeMatrixMultVector(omeMatrix *m, omeVector *v, omeVector *res);
//void omeMatrixSendAsUniform(omeMatrix *m, omeProgram *sp, char *name);
void omeMatrixPrint(const omeMatrix *m);


#ifdef __cplusplus
}
#endif


#endif //OME_MATRIX_H
