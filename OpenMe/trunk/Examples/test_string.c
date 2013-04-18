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

    str1 = omeStringCreate(3);
    assert(omeStringGetLen(str1) == 0);
    omeStringAppendStr(str1, "azertyuiop");
    assert(omeStringCmpStr(str1, "azertyuiop") == 0);

    str2 = omeStringCreateFromStr("Hello World!");
    assert(omeStringCmpStr(str2, "Hello World!") == 0);

    str3 = omeStringCreateFromStr("Hello World! %d", 42);
    assert(omeStringCmpStr(str3, "Hello World! 42") == 0);
    omeStringAppend(str3, str1);
    assert(omeStringCmpStr(str3, "Hello World! 42azertyuiop") == 0);

    str4 = omeStringCopy(str3);
    assert(omeStringCmp(str3, str4) == 0);
    assert(omeStringGetLen(str3) == omeStringGetLen(str4));

    omeStringClear(str1);
    omeStringClear(str2);
    omeStringClear(str3);
    omeStringClear(str4);

    assert(omeStringCmpStr(str1, "") == 0);
    assert(omeStringGetLen(str1) == 0);
    assert(omeStringCmpStr(str2, "") == 0);
    assert(omeStringGetLen(str2) == 0);
    assert(omeStringCmpStr(str3, "") == 0);
    assert(omeStringGetLen(str3) == 0);
    assert(omeStringCmpStr(str4, "") == 0);
    assert(omeStringGetLen(str4) == 0);

    omeStringDestroy(&str1);
    omeStringDestroy(&str2);
    omeStringDestroy(&str3);
    omeStringDestroy(&str4);

    return EXIT_SUCCESS;
}
