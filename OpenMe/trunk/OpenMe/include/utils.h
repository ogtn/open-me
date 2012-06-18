//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_UTILS_H
#define OME_UTILS_H


// types
typedef enum omeStatus
{
    OME_SUCCESS,
    OME_FAILURE = -1
} omeStatus;


typedef enum omeBool
{
    OME_FALSE,
    OME_TRUE
} omeBool;


typedef enum omeType
{
    OME_BYTE,
    OME_UBYTE,
    OME_SHORT,
    OME_USHORT,
    OME_INT,
    OME_UINT,
    OME_FLOAT,
    OME_DOUBLE
} omeType;


// math utils
#define OME_PI              3.141592653589793238462643
#define OME_PIF             3.141592653589793238462643f

#define degToRad(angle)     ((angle) * OME_PI / 180)
#define radToDeg(angle)     ((angle) / OME_PI * 180)

#define degToRadf(angle)    ((angle) * OME_PIF / 180)
#define radToDegf(angle)    ((angle) / OME_PIF * 180)


// macros and functions
#define OME_BUFFER_OFFSET(i) ((char *)NULL + i)
int omeSizeOf(omeType type);
char *omeGetGLError(void);


// other stuff...
#define OME_NAME_MAXLEN     256  


#endif // OME_UTILS_H