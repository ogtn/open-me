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
typedef struct omeString
{
    char *str;		// content of the string
    size_t size;	// allocated size
    size_t length;	// length of the string currently stored
} omeString;


//////////////////////// [Public interface] ////////////////////////

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
int omeStringCmp(omeString *s, omeString *s2);

// Return the extension if the string contain a filename, NULL instead TODO: move this somewhere else?
const char *omeStringGetExtension(const omeString *s);

//////////////////////// [Private functions] ////////////////////////

// Realloc the string with a power of two size
void omeStringResize(omeString *s, size_t needed);

// Compute the size need for holding the result of a vprintf call
size_t countSprintf(const char *format, va_list ap) OME_CHK_FMT(1);


#ifdef __cplusplus
}
#endif


#endif // OME_STRING_H
