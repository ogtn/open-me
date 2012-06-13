//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "vector.h"
#include <math.h>


float omeVectorLength(omeVector *v)
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


void omeVectorAddVector(omeVector *v, omeVector *v2, omeVector *res)
{
    res->x = v->x + v2->x;
    res->y = v->y + v2->y;
    res->z = v->z + v2->z;
}


void omeVectorSubVector(omeVector *v, omeVector *v2, omeVector *res)
{
    res->x = v->x - v2->x;
    res->y = v->y - v2->y;
    res->z = v->z - v2->z;
}


void omeVectorMultVector(omeVector *v, omeVector *v2, omeVector *res)
{
    res->x = v->x * v2->x;
    res->y = v->y * v2->y;
    res->z = v->z * v2->z;
}


void omeVectorMultScal(omeVector *v, float f, omeVector *res)
{
    res->x = v->x * f;
    res->y = v->y * f;
    res->z = v->z * f;
}


float omeVectorDot(omeVector *v, omeVector *v2)
{
    return v->x * v2->x + v->y * v2->y + v->z * v2->z;
}


void omeVectorCross(omeVector *v, omeVector *v2, omeVector *res)
{
    res->x = v->y * v2->z - v->z * v2->y;
    res->y = v->z * v2->x - v->x * v2->z;
    res->z = v->x * v2->y - v->y * v2->x;
}
