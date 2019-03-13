//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "vector.h"
#include "logger.h"
#include <math.h>
#include <string.h>


void omeVectorCopy(omeVector *dst, const omeVector *src)
{
    memcpy(dst, src, sizeof(omeVector));    
}


void omeVector2Copy(omeVector2 *dst, const omeVector2 *src)
{
    memcpy(dst, src, sizeof(omeVector2));
}


float omeVectorLength(const omeVector *v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}


void omeVectorNormalize(omeVector *v)
{
    float norm = omeVectorLength(v);

    if(norm != 0)
    {
        v->x /= norm;
        v->y /= norm;
        v->z /= norm;
    }
}


void omeVectorAddVector(const omeVector *v, const omeVector *v2, omeVector *res)
{
    res->x = v->x + v2->x;
    res->y = v->y + v2->y;
    res->z = v->z + v2->z;
}


void omeVector2AddVector(const omeVector2 *v, const omeVector2 *v2, omeVector2 *res)
{
    res->x = v->x + v2->x;
    res->y = v->y + v2->y;
}


void omeVectorSubVector(const omeVector *v, const omeVector *v2, omeVector *res)
{
    res->x = v->x - v2->x;
    res->y = v->y - v2->y;
    res->z = v->z - v2->z;
}


void omeVectorMultVector(const omeVector *v, const omeVector *v2, omeVector *res)
{
    res->x = v->x * v2->x;
    res->y = v->y * v2->y;
    res->z = v->z * v2->z;
}


void omeVectorMultScal(const omeVector *v, float f, omeVector *res)
{
    res->x = v->x * f;
    res->y = v->y * f;
    res->z = v->z * f;
}


void omeVector2MultScal(const omeVector2 *v, float f, omeVector2 *res)
{
    res->x = v->x * f;
    res->y = v->y * f;
}


float omeVectorDot(const omeVector *v, const omeVector *v2)
{
    return v->x * v2->x + v->y * v2->y + v->z * v2->z;
}


void omeVectorCross(const omeVector *v, const omeVector *v2, omeVector *res)
{
#ifdef _DEBUG
    if(v == res || v2 == res)
        omeLoggerLog("Bad use of omeVectorCross()");
#endif

    res->x = v->y * v2->z - v->z * v2->y;
    res->y = v->z * v2->x - v->x * v2->z;
    res->z = v->x * v2->y - v->y * v2->x;
}
