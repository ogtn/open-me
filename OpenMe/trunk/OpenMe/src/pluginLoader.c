//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "pluginLoader.h"
#ifdef __linux__
#include <dlfcn.h>
#elif _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


omePlugin *omePluginLoad(const char *name)
{
    omePluginInitFunc init = NULL;
    omePlugin *p;

    p = calloc(1, sizeof(omePlugin));

#ifdef __linux
    dlerror(); // clear previous error

    // RTLD_NOW: no lazy opening, all symbols are loaded when dlopen() returns
    // RTLD_GLOBAL: symbols loaded will be available even from libraries loaded after
    p->handle = dlopen(name, RTLD_NOW | RTLD_GLOBAL);


    if(p->handle == NULL)
    {
        const char *msg = dlerror();

        fprintf(stderr, "Error occured while loading library %s: %s\n", name, msg ? msg : "unknown error");
    }
#elif _WIN32
    //TODO: wchar_t for windows ... or use LoadLibraryA() (bad idea)
    p->handle = LoadLibrary(name);
#endif

    init = omePluginGetFunc(p, "omePluginInit");

    if(init)
        p->type = init();
    else
        omePluginUnload(&p);

    return p;
}


void omePluginUnload(omePlugin **p)
{
    omePluginQuitFunc quit;

    quit = omePluginGetFunc(*p, "omePluginQuit");

    if(quit != NULL)
        quit();

#ifdef __linux__
    dlerror(); // clear previous error

    if(dlclose((*p)->handle))
    {
        const char *msg = dlerror();

        fprintf(stderr, "Error occured while unloading library %s: %s\n", (*p)->name, msg ? msg : "unknown error");
    }
#elif _WIN32
    FreeLibrary((*p)->handle);
#endif

    memset(*p, 0, sizeof(omePlugin));
    free(*p);
    *p = 0;
}


void *omePluginGetFunc(omePlugin *p, const char *funcName)
{
    void *res;

    if(!p->handle)
        return NULL;

#ifdef __linux__
    dlerror(); // clear previous error
    res = dlsym(p->handle, funcName);

    if(res == NULL)
    {
        const char *msg = dlerror();

        fprintf(stderr, "Error occured while loading symbol %s from library %s: %s\n", funcName, p->name, msg ? msg : "unknown error");
    }
#elif _WIN32
    res = GetProcAddress(p->handle, funcName);
#endif

    return res;
}
