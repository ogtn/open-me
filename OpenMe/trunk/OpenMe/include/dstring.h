//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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
    unsigned int size;
    unsigned length;
} omeString;


omeString *omeStringCreate(unsigned int needed);

#ifdef __GNUC__
omeString *omeStringCreateFromStr(const char *format, ...) __attribute((format(printf, 1, 2)));
#else
omeString *omeStringCreateFromStr(const char *format, ...);
#endif

omeString *omeStringCopy(const omeString *s);
void omeStringDestroy(omeString **s);
void omeStringRealloc(omeString *s, unsigned int needed);
void omeStringFit(omeString *s);
omeString *omeStringClear(omeString *s);
omeString *omeStringAppend(omeString *s, const omeString *s2);

#ifdef __GNUC__
omeString *omeStringAppendStr(omeString *s, const char *format, ...) __attribute((format(printf, 2, 3)));
#else
omeString *omeStringAppendStr(omeString *s, const char *format, ...);
#endif

const char *omeStringGetExtension(const omeString *s);

#ifdef __GNUC__
unsigned int countSprintf(const char *format, va_list ap) __attribute((format(printf, 1, 0)));
#else
unsigned int countSprintf(const char *format, va_list ap);
#endif

#ifdef __cplusplus
}
#endif


#endif // OME_STRING_H
