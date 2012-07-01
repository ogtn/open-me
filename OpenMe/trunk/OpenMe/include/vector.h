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


#ifdef __cplusplus
extern "C" {
#endif

    
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


typedef union omeVector2
{
    struct
    {
        float x;
        float y;
    };

    float tab[2];
} omeVector2;


void omeVectorCopy(omeVector *dst, const omeVector *src);
void omeVector2Copy(omeVector2 *dst, const omeVector2 *src);
float omeVectorLength(const omeVector *v);
void omeVectorNormalize(omeVector *v);
void omeVectorAddVector(const omeVector *v, const omeVector *v2, omeVector *res);
void omeVector2AddVector(const omeVector2 *v, const omeVector2 *v2, omeVector2 *res);
void omeVectorSubVector(const omeVector *v, const omeVector *v2, omeVector *res);
void omeVectorMultVector(const omeVector *v, const omeVector *v2, omeVector *res);
void omeVectorMultScal(const omeVector *v, float f, omeVector *res);
void omeVector2MultScal(const omeVector2 *v, float f, omeVector2 *res);
float omeVectorDot(const omeVector *v, const omeVector *v2);
void omeVectorCross(const omeVector *v, const omeVector *v2, omeVector *res);


#ifdef __cplusplus
}
#endif


#endif // OME_VECTOR_H
