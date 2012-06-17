//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_OPENME_H
#define OME_OPENME_H


#include "utils.h"
#include "engine.h"
#include "matrix.h"
#include "camera.h"
#include "buffer.h"
#include "mesh.h"


//TODO: avoid memset() calls in *Destroy functions in release
//TODO: test *alloc() return in debug mode, through "overloading"?
//TODO: add an omeBool field in structures, so they can know if they are dynamic ok not and avoid calling free()??? Not sure if stupid....
//TODO: add extern C in every header?
//TODO: make sure all static stuffs are const, to allow multithreading
//TODO: use glewIsSupported() to adapt the render to the hardware's capabilities


#endif // OME_OPENME_H
