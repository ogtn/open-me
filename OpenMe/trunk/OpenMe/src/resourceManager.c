//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "resourceManager.h"
#include "program.h"
#include "texture.h"
#include "logger.h"


static omeResourceManager manager = {NULL};


void omeResourceInitialize(omeResource *r, omeResourceType type, const char *name)
{
    r->magic = OME_RESOURCE_MAGIC;
    r->type = type;
    r->refCount = 1;
    
    // anonymous resources don't need to be in the hash table
    if(name == NULL)
        r->anonymous = OME_TRUE;
    else
    {
        r->anonymous = OME_FALSE;
        strncpy(r->hashKey, name, OME_NAME_MAXLEN);
        HASH_ADD_STR(manager.resources, hashKey, r);
    }
}


// TODO: maybe should it be a macro...
omeBool omeResourceCheck(omeResource *r)
{
    if(r->magic != OME_RESOURCE_MAGIC)
    {
        omeLoggerLog("This is not a valid resource\n");
        return OME_FALSE;
    }

    return OME_TRUE;
}


void *omeResourceFind(const char *name)
{
    omeResource *r;

    HASH_FIND_STR(manager.resources, name, r);

    return r;
}


void omeResourceAddRef(omeResource *r)
{
    if(omeResourceCheck(r))
        r->refCount++;
}


void omeResourceDelRef(omeResource *r)
{
    if(omeResourceCheck(r))
    {
        r->refCount--;

        if(r->refCount == 0)
        {
            if(r->anonymous == OME_FALSE)
                HASH_DEL(manager.resources, r);

            switch(r->type)
            {
            case OME_RESOURCE_TEXTURE:
                omeTextureDestroy((omeTexture**)&r);
                break;
            case OME_RESOURCE_PIXEL_SHADER:
            case OME_RESOURCE_VERTEX_SHADER:
                omeShaderDestroy((omeShader**)&r);
                break;
            case OME_RESOURCE_MATERIAL:
                break;
            case OME_RESOURCE_GEOMETRY:
                break;
            default:
                break;
            }
        }
    }
}
