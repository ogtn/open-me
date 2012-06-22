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
#include <stdlib.h>


omePlugin *omePluginLoad(const char *name)
{
    omePluginInitFunc init = NULL;
    omePlugin *p;

    p = calloc(1, sizeof(omePlugin));

#ifdef __linux__
    p->handle = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
    //TODO: check those weird flags...
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

    if(quit)
        quit();

#ifdef __linux__
    dlclose((*p))->handle); 
#elif _WIN32
    FreeLibrary((*p)->handle);
#endif

    memset(*p, 0, sizeof(omePlugin));
    free(*p);
    *p = 0;
}


void *omePluginGetFunc(omePlugin *p, const char *funcName)
{
    if(!p->handle)
        return NULL;

#ifdef __linux__
    return dlsym(p->handle, funcName); 
#elif _WIN32
    return GetProcAddress(p->handle, funcName);
#endif
}
