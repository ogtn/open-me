//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "dstring.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


typedef struct omeString
{
    char *str;      // content of the string
    size_t size;    // allocated size
    size_t length;  // length of the string currently stored
} omeString;


// Realloc the string with a power of two size
static void omeStringResize(omeString *s, size_t needed);

// Compute the size need for holding the result of a vprintf call
static size_t countSprintf(const char *format, va_list ap) OME_CHK_FMT(1);


omeString *omeStringCreate(size_t needed)
{
    omeString *s = calloc(1, sizeof (omeString));

    omeStringResize(s, needed);
    s->str[0] = '\0';

    return s;
}


omeString *omeStringCopy(const omeString *s)
{
    omeString *res = calloc(1, sizeof (omeString));

    if(s != NULL)
    {
        res->length = s->length;
        omeStringResize(res, s->length);
        strcpy(res->str, s->str);
    }

    return res;
}


omeString *omeStringCreateFromStr(const char *format, ...)
{
    va_list list;
    omeString *s = calloc(1, sizeof (omeString));

    va_start(list, format);
    s->length = countSprintf(format, list);
    va_end(list);
    
    omeStringResize(s, s->length);

    va_start(list, format);
    vsnprintf(s->str, s->length + 1, format, list);
    va_end(list);

    return s;
}


void omeStringDestroy(omeString **s)
{
    if((*s)->size != 0)
        free((*s)->str);

    omeDbgClearMem(*s, sizeof(omeString));
    free(*s);
    *s = NULL;
}


void omeStringResize(omeString *s, size_t needed)
{
    s->size = omeNextPowOfTwo(needed + 1);
    s->str = realloc(s->str, s->size);
}


void omeStringFit(omeString *s)
{
    s->size = s->length + 1;
    s->str = realloc(s->str, s->size);
}


omeString *omeStringClear(omeString *s)
{
    s->length = 0;
    s->str[0] = '\0';

    return s;
}


omeString *omeStringAppend(omeString *s, const omeString *s2)
{
    size_t old_len = s->length;

    s->length += s2->length;

    if(s->length + 1 > s->size)
        omeStringResize(s, s->length);
    
    strcpy(&s->str[old_len], s2->str);
    s->str[s->length] = '\0';

    return s;
}


omeString *omeStringAppendStr(omeString *s, const char *format, ...)
{
    va_list list;
    int formaLen;
    char *end = &s->str[s->length];

    va_start(list, format);
    formaLen = countSprintf(format, list);
    s->length += formaLen;
    va_end(list);

    if(s->length + 1 > s->size)
        omeStringResize(s, s->length);

    va_start(list, format);
    vsnprintf(end, s->length + 1, format, list);
    va_end(list);

    return s;
}


int omeStringCmp(const omeString *s, const omeString *s2)
{
    return strcmp(s->str, s2->str);
}


int omeStringCmpStr(const omeString *s, const char *str)
{
    return strcmp(s->str, str);
}


const char *omeStringGetStr(const omeString *s)
{
    return s->str;
}


size_t omeStringGetLen(const omeString *s)
{
    return s->length;
}


const char *omeStringGetExtension(const omeString *s)
{
    char *c = s->str + s->length;

    while(c != s->str)
        if(*c == '.')
            return c++;

    return NULL;
}


// TODO: find a better solution...
// see http://stackoverflow.com/questions/5184459/msvc-linux-code-for-count-sprintf
size_t countSprintf(const char *format, va_list ap)
{
#ifdef WIN32
	return _vscprintf(format, ap);
#else
	char c;
	return vsnprintf(&c, 1, format, ap);
#endif
}
