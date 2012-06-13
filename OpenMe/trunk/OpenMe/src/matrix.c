//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "matrix.h"
#include "constants.h"
#include <string.h>
#include <math.h>


static const float identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1};


void omeMatrixMakeIdentity(omeMatrix *m)
{
    memcpy(m, identity, sizeof(identity));
}


void omeMatrixMakeOrtho(omeMatrix *m, float left, float right, float bottom, float top, float zNear, float zFar)
{
    omeMatrixMakeIdentity(m);

    m->data[0][0] = 2 / (right - left);
    m->data[0][3] = -(right + left) / (right - left);

    m->data[1][1] = 2 / (top - bottom);
    m->data[1][3] = -(top + bottom) / (top - bottom);

    m->data[2][2] = 2 / (zFar - zNear);
    m->data[2][3] = -(zFar + zNear) / (zFar - zNear);
}


void omeMatrixMakePerspective(omeMatrix *m, float angle, float ratio, float near, float far)
{
    float f;

    omeMatrixMakeIdentity(m);

    angle *= (float)OME_PI / 180;
    f = 1 / tan(angle / 2);

    m->data[0][0] = f / ratio;
    m->data[1][1] = f;
    m->data[2][2] = (near + far) / (near - far);
    m->data[3][3] = 0;
    m->data[2][3] = (2 * near * far) / (near - far);
    m->data[3][2] = -1;
}


void omeMatrixMakeLookAt(omeMatrix *m, omeVector *pos, omeVector *target, omeVector *up)
{
    omeVector forward, side, newUP;

    omeMatrixMakeIdentity(m);

    // forward vector
    omeVectorSubVector(target, pos, &forward);
    omeVectorNormalize(&forward);

    // side vector
    omeVectorNormalize(up);
    omeVectorMultVector(&forward, up, &side);
    omeVectorNormalize(&side);

    // corrected up vector
    omeVectorMultVector(&side, &forward, &newUP);

    m->data[0][0] = side.x;
    m->data[0][1] = side.y;
    m->data[0][2] = side.z;

    m->data[1][0] = newUP.x;
    m->data[1][1] = newUP.y;
    m->data[1][2] = newUP.z;

    m->data[2][0] = -forward.x;
    m->data[2][1] = -forward.y;
    m->data[2][2] = -forward.z;
    m->data[2][3] = omeVectorDot(&forward, pos);  // wtf, again...
}


void omeMatrixTranslate(omeMatrix *m, omeVector *v)
{
    omeMatrix m2;

    omeMatrixMakeIdentity(&m2);

    m2.data[0][3] = v->x;
    m2.data[1][3] = v->y;
    m2.data[2][3] = v->z;

    omeMatrixMultMatrix(m, &m2, m);
}


void omeMatrixScale(omeMatrix *m, omeVector *v)
{
    omeMatrix m2;

    omeMatrixMakeIdentity(&m2);

    m2.data[0][0] = v->x;
    m2.data[1][1] = v->y;
    m2.data[2][2] = v->z;

    omeMatrixMultMatrix(m, &m2, m);
}

/*
void omeMatrixRotate(omeMatrix *m, omeVector *axis, float theta)
{
    omeVector v;
    omeMatrix m2;
    float ct, st;

    theta = degToRad(theta);
    ct = cosf(theta);
    st = sinf(theta);
       
    memcpy(&v, axis, sizeof(omeVector)); // TODO: omeCopy() macro??
    omeVectorNormalize(&v);
    m.makeIdentity();
    
    m.data[0][0] = v.x * v.x * (1 - ct) + ct;
    m.data[0][1] = v.x * v.y * (1 - ct) - v.z * st;
    m.data[0][2] = v.x * v.z * (1 - ct) + v.y * st;
    
    m.data[1][0] = v.x * v.y * (1 - ct) + v.z * st;
    m.data[1][1] = v.y * v.y * (1 - ct) + ct;
    m.data[1][2] = v.y * v.z * (1 - ct) - v.x * st;
    
    m.data[2][0] = v.x * v.z * (1 - ct) - v.y * st;
    m.data[2][1] = v.y * v.z * (1 - ct) + v.x * st;
    m.data[2][2] = v.z * v.z * (1 - ct) + ct;
    
    return *this = *this * m;
}
*/

//void omeMatrixRotate(omeMatrix *m, omeVector *v);
//void omeMatrixTranspose(omeMatrix *m, omeVector *v);
//void omeMatrixTranslate(omeMatrix *m, omeVector *v);
//void omeMatrixLoad(omeMatrix *m);
//void omeMatrixLoadTransp(omeMatrix *m);


void omeMatrixMultMatrix(omeMatrix *m, omeMatrix *m2, omeMatrix *res)
{
    int i, j, k;

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            res->data[i][j] = 0;

            for(k = 0; k < 4; k++)
                res->data[i][j] += m->data[i][k] * m2->data[k][j];
        }
    }
}


//void omeMatrixMultVector(omeMatrix *m, omeVector *v, omeVector *res);
//void omeMatrixSendAsUniform(omeMatrix *m, omeShaderProgram *sp, char *name);
