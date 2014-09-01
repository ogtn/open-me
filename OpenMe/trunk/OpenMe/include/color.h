//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_COLOR_H
#define OME_COLOR_H


#ifdef __cplusplus
extern "C" {
#endif


typedef union omeColor
{
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };

    float tab[4];
} omeColor;


typedef union omeColor3
{
    struct
    {
        float r;
        float g;
        float b;
    };

    float tab[3];
} omeColor3;


typedef union omeColori
{
    struct
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    unsigned char tab[4];
} omeColori;


#ifdef __cplusplus
}
#endif


#endif // OME_COLOR_H
