//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "dstring.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


omeString *omeStringCreate(unsigned int needed)
{
    omeString *s = calloc(1, sizeof (omeString));

    s->length = needed;
    omeStringRealloc(s, s->length + 1);
    s->str[0] = '\0';

    return s;
}


omeString *omeStringCopy(const omeString *s)
{
    omeString *res = calloc(1, sizeof (omeString));

    if(s != NULL)
    {
        res->length = s->length;
        omeStringRealloc(res, s->length + 1);
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
    
    omeStringRealloc(s, s->length + 1);

    va_start(list, format);
    vsnprintf(s->str, s->length + 1, format, list);
    va_end(list);

    return s;
}


void omeStringDestroy(omeString **s)
{
    if((*s)->size != 0)
        free((*s)->str);

    memset(*s, 0, sizeof(omeString));
    free(*s);
    *s = NULL;
}


void omeStringRealloc(omeString *s, unsigned int needed)
{
    s->size = omeNextPowOfTwo(needed);
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
    s->length += s2->length;

    if(s->length + 1 > s->size)
        omeStringRealloc(s, s->length + 1);
    
    strcpy(&s->str[s->length], s2->str);

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
        omeStringRealloc(s, s->length + 1);

    va_start(list, format);
    vsnprintf(end, s->length + 1, format, list);
    va_end(list);

    return s;
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
unsigned int countSprintf(const char *format, va_list ap)
{
#ifdef WIN32
	return _vscprintf(format, ap);
#else
	char c;
	return vsnprintf(&c, 1, format, ap);
#endif
}
