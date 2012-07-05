//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "matrix.h"
#include "utils.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <GL/glew.h>


static const float identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1};


void omeMatrixCopy(omeMatrix *dst, const omeMatrix *src)
{
    memcpy(dst, src, sizeof(omeMatrix));
}


void omeMatrixMakeIdentity(omeMatrix *m)
{
    memcpy(m, identity, sizeof(omeMatrix));
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

    angle = degToRadf(angle);
    f = 1 / tanf(angle / 2);

    m->data[0][0] = f / ratio;
    m->data[1][1] = f;
    m->data[2][2] = (near + far) / (near - far);
    m->data[3][3] = 0;
    m->data[2][3] = (2 * near * far) / (near - far);
    m->data[3][2] = -1;
}


void omeMatrixMakeLookAt(omeMatrix *m, const omeVector *pos, const omeVector *target, const omeVector *up)
{
    omeVector forward, side;
    omeVector newUp = *up;

    omeMatrixMakeIdentity(m);

    // forward vector
    omeVectorSubVector(target, pos, &forward);
    omeVectorNormalize(&forward);

    // side vector
    omeVectorNormalize(&newUp);
    omeVectorCross(&forward, &newUp, &side);

    // corrected up vector
    omeVectorNormalize(&side);
    omeVectorCross(&side, &forward, &newUp);

    m->data[0][0] = side.x;
    m->data[0][1] = side.y;
    m->data[0][2] = side.z;

    m->data[1][0] = newUp.x;
    m->data[1][1] = newUp.y;
    m->data[1][2] = newUp.z;

    m->data[2][0] = -forward.x;
    m->data[2][1] = -forward.y;
    m->data[2][2] = -forward.z;
    m->data[2][3] = omeVectorDot(&forward, pos);  // wtf, again...
}


void omeMatrixTranslate(omeMatrix *m, const omeVector *v)
{
    omeMatrix m2;

    omeMatrixMakeIdentity(&m2);

    m2.data[0][3] = v->x;
    m2.data[1][3] = v->y;
    m2.data[2][3] = v->z;

    omeMatrixMultMatrix(m, &m2, m);
}


void omeMatrixScale(omeMatrix *m, const omeVector *v)
{
    omeMatrix m2;

    omeMatrixMakeIdentity(&m2);

    m2.data[0][0] = v->x;
    m2.data[1][1] = v->y;
    m2.data[2][2] = v->z;

    omeMatrixMultMatrix(m, &m2, m);
}


void omeMatrixRotateAxis(omeMatrix *m, const omeVector *axis, float theta)
{
    omeVector v;
    omeMatrix m2;
    float ct, st;

    theta = degToRadf(theta);
    ct = cosf(theta);
    st = sinf(theta);
       
    omeVectorCopy(&v, axis);
    omeVectorNormalize(&v);
    omeMatrixMakeIdentity(&m2);
    
    m2.data[0][0] = v.x * v.x * (1 - ct) + ct;
    m2.data[0][1] = v.x * v.y * (1 - ct) - v.z * st;
    m2.data[0][2] = v.x * v.z * (1 - ct) + v.y * st;
    
    m2.data[1][0] = v.x * v.y * (1 - ct) + v.z * st;
    m2.data[1][1] = v.y * v.y * (1 - ct) + ct;
    m2.data[1][2] = v.y * v.z * (1 - ct) - v.x * st;
    
    m2.data[2][0] = v.x * v.z * (1 - ct) - v.y * st;
    m2.data[2][1] = v.y * v.z * (1 - ct) + v.x * st;
    m2.data[2][2] = v.z * v.z * (1 - ct) + ct;
    
    omeMatrixMultMatrix(m, &m2, m);
}


void omeMatrixRotateAngles(omeMatrix *m, const omeVector *v)
{
    omeVector sinv, cosv;
    omeMatrix m2;

    sinv.x = sinf(degToRad(v->x));
    cosv.x = cosf(degToRad(v->x));
    sinv.y = sinf(degToRad(v->y));
    cosv.y = cosf(degToRad(v->y));
    sinv.z = sinf(degToRad(v->z));
    cosv.z = cosf(degToRad(v->z));

    omeMatrixMakeIdentity(&m2);

    m2.data[0][0] = cosv.y * cosv.z;
    m2.data[0][1] = -cosv.x * sinv.z + sinv.x * sinv.y * cosv.z;
    m2.data[0][2] = sinv.x * sinv.z + cosv.x * sinv.y * cosv.z;
    
    m2.data[1][0] = cosv.y * sinv.z;
    m2.data[1][1] = cosv.x * cosv.z + sinv.x * sinv.y * sinv.z;
    m2.data[1][2] = -sinv.x * cosv.z + cosv.x * sinv.y * sinv.z;
    
    m2.data[2][0] = -sinv.y;
    m2.data[2][1] = sinv.x * cosv.y;
    m2.data[2][2] = cosv.x * cosv.y;
    
    omeMatrixMultMatrix(m, &m2, m);
}


void omeMatrixMakeTransfo(omeMatrix *m, const omeVector *position, const omeVector *rotation, const omeVector *scaling)
{
    omeVector sinv, cosv;

    sinv.x = sinf(degToRad(rotation->x));
    cosv.x = cosf(degToRad(rotation->x));
    sinv.y = sinf(degToRad(rotation->y));
    cosv.y = cosf(degToRad(rotation->y));
    sinv.z = sinf(degToRad(rotation->z));
    cosv.z = cosf(degToRad(rotation->z));

    // rotation
    m->data[0][0] = cosv.y * cosv.z;
    m->data[0][1] = -cosv.x * sinv.z + sinv.x * sinv.y * cosv.z;
    m->data[0][2] = sinv.x * sinv.z + cosv.x * sinv.y * cosv.z;
    
    m->data[1][0] = cosv.y * sinv.z;
    m->data[1][1] = cosv.x * cosv.z + sinv.x * sinv.y * sinv.z;
    m->data[1][2] = -sinv.x * cosv.z + cosv.x * sinv.y * sinv.z;
    
    m->data[2][0] = -sinv.y;
    m->data[2][1] = sinv.x * cosv.y;
    m->data[2][2] = cosv.x * cosv.y;

    // scaling
    m->data[0][0] *= scaling->x;
    m->data[1][0] *= scaling->x;
    m->data[2][0] *= scaling->x;
    m->data[0][1] *= scaling->y;
    m->data[1][1] *= scaling->y;
    m->data[2][1] *= scaling->y;
    m->data[0][2] *= scaling->z;
    m->data[1][2] *= scaling->z;
    m->data[2][2] *= scaling->z;

    // translation
    m->data[0][3] = position->x;
    m->data[1][3] = position->y;
    m->data[2][3] = position->z;

    // remaining data
    m->data[3][0] = m->data[3][1] = m->data[3][2] = 0;
    m->data[3][3] = 1;
}


void omeMatrixTranspose(omeMatrix *m)
{
    int i, j;
    omeMatrix m2;

    omeMatrixCopy(&m2, m);
    
    for(i = 0; i < 4; i++)
        for(j = 0; j < 4; j++)
            m->data[i][j] = m2.data[j][i];
}


void omeMatrixLoad(const omeMatrix *m, int transpose)
{
    omeMatrix m2;

    if(transpose)
    {
        omeMatrixCopy(&m2, m);
        omeMatrixTranspose(&m2);
    }
    
    glLoadMatrixf(m2.tab);
}


void omeMatrixMultMatrix(const omeMatrix *m, const omeMatrix *m2, omeMatrix *res)
{
    int i, j, k;
    omeMatrix tmp;
    omeMatrix *m3 = res;

    // self affectation
    if(m == res)
        m3 = &tmp;

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
        {
            m3->data[i][j] = 0;

            for(k = 0; k < 4; k++)
                m3->data[i][j] += m->data[i][k] * m2->data[k][j];
        }
    }

    // self affectation
    if(m == res)
        omeMatrixCopy(res, m3);
}


//TODO: implement those functions...
//void omeMatrixMultVector(omeMatrix *m, omeVector *v, omeVector *res);
//void omeMatrixSendAsUniform(omeMatrix *m, omeProgram *sp, char *name);


void omeMatrixPrint(const omeMatrix *m)
{
    int i, j;

    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 4; j++)
            printf("%.2f ", m->data[i][j]);

        printf("\n");
    }

    printf("\n");
}
