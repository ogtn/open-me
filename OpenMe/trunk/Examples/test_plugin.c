//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include <openme.h>
#include "assert.h"


typedef int(*plugin_func)(int);


int main(void)
{
	omePlugin *plugin = NULL;
	plugin_func func = NULL;

	plugin = omePluginLoad("plugin.so");
	assert(plugin != NULL);
	
	func = omePluginGetFunc(plugin, "func_double");
	assert(func != NULL);
	assert(func(42) == 84);

	omePluginUnload(&plugin);
	assert(plugin == NULL);

	return EXIT_SUCCESS;
}
