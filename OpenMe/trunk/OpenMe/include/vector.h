//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_VECTOR_H
#define OME_VECTOR_H


typedef union omeVector
{
    struct
    {
        float x;
        float y;
        float z;
    };

    float tab[3];
} omeVector;


void omeVectorCopy(omeVector *dst, omeVector *src);
float omeVectorLength(omeVector *v);
void omeVectorNormalize(omeVector *v);
void omeVectorAddVector(omeVector *v, omeVector *v2, omeVector *res);
void omeVectorSubVector(omeVector *v, omeVector *v2, omeVector *res);
void omeVectorMultVector(omeVector *v, omeVector *v2, omeVector *res);
void omeVectorMultScal(omeVector *v, float f, omeVector *res);
float omeVectorDot(omeVector *v, omeVector *v2);
void omeVectorCross(omeVector *v, omeVector *v2, omeVector *res);


#endif // OME_VECTOR_H
