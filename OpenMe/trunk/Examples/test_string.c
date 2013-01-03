//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "dstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int main(void)
{
    omeString *str1, *str2, *str3, *str4;

    str1 = omeStringCreate(42);
    assert(str1->length == 42);
    assert(strlen(str1->str) == 0);

    str2 = omeStringCreateFromStr("Hello World!");
    assert(strcmp(str2->str, "Hello World!") == 0);


    str3 = omeStringCreateFromStr("Hello World! %d", 42);
    assert(strcmp(str3->str, "Hello World! 42") == 0);

    str4 = omeStringCopy(str3);
    assert(strcmp(str3->str, str4->str) == 0);
    assert(str3->length == str4->length);
    assert(str3->size == str4->size);

    omeStringClear(str4);
    assert(str4->length == strlen(str4->str));
/*
void omeStringDestroy(omeString **s);
void omeStringRealloc(omeString *s, int needed);
omeString *omeStringAppend(omeString *s, const omeString *s2);
omeString *omeStringAppendStr(omeString *s, const char *format, ...);
const char *omeStringGetExtension(const omeString *s);
*/
    return EXIT_SUCCESS;
}
