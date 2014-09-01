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


#include "utils.h"
#include <stdarg.h>
#include <stddef.h>


// dynamic string
typedef struct omeString omeString;


// Constructors and destructor
omeString *omeStringCreate(size_t needed);
omeString *omeStringCreateFromStr(const char *format, ...) OME_CHK_FMT_AND_ARGS(1);
omeString *omeStringCopy(const omeString *s);
void omeStringDestroy(omeString **s);

// Reduce the allocated size to the minimum, without losing any character
void omeStringFit(omeString *s);

// Empty the string, wihout freeing memory 
omeString *omeStringClear(omeString *s);

// Append another omeString, or c string
omeString *omeStringAppend(omeString *s, const omeString *s2);
omeString *omeStringAppendStr(omeString *s, const char *format, ...) OME_CHK_FMT_AND_ARGS(2);

// Compare two strings
int omeStringCmp(const omeString *s, const omeString *s2);
int omeStringCmpStr(const omeString *s, const char *str);

// Return the internal string
const char *omeStringGetStr(const omeString *s);

// Return the size of the string, without trailing '\0'
size_t omeStringGetLen(const omeString *s);

// Return the extension if the string contains a filename, NULL instead TODO: move this somewhere else?
const char *omeStringGetExtension(const omeString *s);


#ifdef __cplusplus
}
#endif


#endif // OME_STRING_H
