//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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


// Three dimensional vector, single precision 
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


// Two dimensional vector, single precision
typedef union omeVector2
{
    struct
    {
        float x;
        float y;
    };

    float tab[2];
} omeVector2;


// Copy a vector into another
void omeVectorCopy(omeVector *dst, const omeVector *src);

// Return the length of the given vector
float omeVectorLength(const omeVector *v);

// Return the dot product of two vectors
float omeVectorDot(const omeVector *v, const omeVector *v2);

// Normalize the given vector
void omeVectorNormalize(omeVector *v);

// For the following operations, the result is stored into a res vector, and the other 
// parameters are unchanged. 
// However, it is safe to pass several time the same vector to make a +=, *= etc...
void omeVectorAddVector(const omeVector *v, const omeVector *v2, omeVector *res);
void omeVectorSubVector(const omeVector *v, const omeVector *v2, omeVector *res);
void omeVectorMultVector(const omeVector *v, const omeVector *v2, omeVector *res);
void omeVectorMultScal(const omeVector *v, float f, omeVector *res);

// Return the cross product of two vectors. 
// Do NOT pass the same vector to v or v2 to res.
void omeVectorCross(const omeVector *v, const omeVector *v2, omeVector *res);

// 2D equivalents of the above functions
void omeVector2Copy(omeVector2 *dst, const omeVector2 *src);
void omeVector2AddVector(const omeVector2 *v, const omeVector2 *v2, omeVector2 *res);
void omeVector2MultScal(const omeVector2 *v, float f, omeVector2 *res);


#ifdef __cplusplus
}
#endif


#endif // OME_VECTOR_H
