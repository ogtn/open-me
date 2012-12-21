//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_STRING_H
#define OME_STRING_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stdarg.h>


typedef struct omeString
{
    char *str;
    int size;
    int length;
} omeString;


omeString *omeStringCreate(int needed);
omeString *omeStringCreateFromStr(const char *format, ...);
omeString *omeStringCopy(const omeString *s);
void omeStringDestroy(omeString **s);
void omeStringRealloc(omeString *s, int needed);
void omeStringFit(omeString *s);
omeString *omeStringClear(omeString *s);
omeString *omeStringAppend(omeString *s, const omeString *s2);
omeString *omeStringAppendStr(omeString *s, const char *format, ...);
const char *omeStringGetExtension(const omeString *s);

int countSprintf(const char *format, va_list ap);


#ifdef __cplusplus
}
#endif


#endif // OME_STRING_H
