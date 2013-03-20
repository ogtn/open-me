//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_PLUGIN_INTERFACE_H
#define OME_PLUGIN_INTERFACE_H


#ifdef __cplusplus
extern "C" {
#endif


typedef enum omePluginType
{
    OME_PLUGIN_TYPE_MESH_LOADER     = 0x01,
    OME_PLUGIN_TYPE_MESH_SAVER      = 0x02,
    OME_PLUGIN_TYPE_MATERIAL_LOADER = 0x04,
    OME_PLUGIN_TYPE_MATERIAL_SAVER  = 0x08,
    OME_PLUGIN_TYPE_TEXTURE_LOADER  = 0x10,
    OME_PLUGIN_TYPE_TEXTURE_SAVER   = 0x20,
} omePluginType;


// function types wich can be imported from plugins
typedef omePluginType (*omePluginInitFunc)(void);
typedef void (*omePluginQuitFunc)(void);


// functions wich need to be implemented
omePluginType omePluginInit(void);
void omePluginQuit(void);


#ifdef __cplusplus
}
#endif


#endif // OME_PLUGIN_INTERFACE_H
