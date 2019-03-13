//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_PLUGIN_LOADER_H
#define OME_PLUGIN_LOADER_H


#ifdef __cplusplus
extern "C" {
#endif


#include "pluginInterface.h"


// Handle to a loaded plugin
typedef struct omePlugin
{
    char *name;				// plugin name
    void *handle;			// linux/windows plugin handle
    omePluginType type;		// type of plugin, see pluginInterface for the
    						// different values and the associated interfaces
} omePlugin;


// load a plugin from its name
// TODO: use extension .omePlugin instead of .dll and .so?
// TODO: only search into a plugins/ folder?
omePlugin *omePluginLoad(const char *name);

// unload the plugin
// TODO: hide this from the user and let the engine deal with plugins
// to avoid access to unloaded data/functions?
void omePluginUnload(omePlugin **p);

// retreive the function pointer associated with the given name
// TODO: not only functions are accessible through this mechanism
void *omePluginGetFunc(omePlugin *p, const char *funcName);


#ifdef __cplusplus
}
#endif


#endif // OME_PLUGIN_LOADER_H
