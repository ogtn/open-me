//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "pluginInterface.h"
#include <stdio.h>


omePluginType omePluginInit(void)
{
	// puts("omePluginInit");

	return OME_PLUGIN_TYPE_TEXTURE_LOADER;
}


void omePluginQuit(void)
{
	// puts("omePluginQuit");
}


int func_double(int n)
{
	return n * 2;
}
