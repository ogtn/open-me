//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_OBJ_LOADER_H
#define OME_OBJ_LOADER_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"


// forward declaration
typedef struct omeMesh omeMesh;


omeMesh *omeLoadOBJFromFile(const char *fileName, omeBool swapYZ);
void omeSaveOmeMeshToFile(const char *filename, omeMesh *m);
omeMesh *omeLoadOmeMeshFromFile(const char *filename);


#ifdef __cplusplus
}
#endif


#endif // OME_OBJ_LOADER_H
