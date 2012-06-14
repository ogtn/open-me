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
#include <stdio.h>
#include <GL/glfw.h>


static const float identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1};


void omeMatrixCopy(omeMatrix *dst, omeMatrix *src)
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

    angle = degToRad(angle);
    f = 1 / tanf(angle / 2);

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
    omeVectorCross(&forward, up, &side);

    // corrected up vector
    omeVectorNormalize(&side);
    omeVectorCross(&side, &forward, &newUP);

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


void omeMatrixRotateAxis(omeMatrix *m, omeVector *axis, float theta)
{
    omeVector v;
    omeMatrix m2;
    float ct, st;

    theta = degToRad(theta);
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


void omeMatrixRotateAngles(omeMatrix *m, omeVector *v)
{
    // TODO: find a less awful way to implement this...
    static omeVector vx = {1, 0, 0};
    static omeVector vy = {0, 1, 0};
    static omeVector vz = {0, 0, 1};
    omeMatrix mx, my, mz;

    omeMatrixCopy(&mx, m);
    omeMatrixCopy(&my, m);
    omeMatrixCopy(&mz, m);

    omeMatrixRotateAxis(&mx, &vx, v->x);
    omeMatrixRotateAxis(&my, &vy, v->y);
    omeMatrixRotateAxis(&mz, &vz, v->z);

    omeMatrixMultMatrix(m, &mx, m);
    omeMatrixMultMatrix(m, &my, m);
    omeMatrixMultMatrix(m, &mz, m);
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


void omeMatrixLoad(omeMatrix *m, int transpose)
{
    omeMatrix m2;

    if(transpose)
    {
        omeMatrixCopy(&m2, m);
        omeMatrixTranspose(&m2);
    }
    
    glLoadMatrixf(m2.tab);
}


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


//TODO: implement those functions...
//void omeMatrixMultVector(omeMatrix *m, omeVector *v, omeVector *res);
//void omeMatrixSendAsUniform(omeMatrix *m, omeShaderProgram *sp, char *name);


void omeMatrixPrint(omeMatrix *m)
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
