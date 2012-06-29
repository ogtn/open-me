//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "scene.h"
#include "camera.h"
#include "geometry.h"
#include "mesh.h"
#include "shaderProgram.h"
#include <utlist.h>
#include <stdlib.h>
#include <string.h>


omeScene *omeSceneCreate(void)
{
    omeScene *s = calloc(1, sizeof(omeScene));

    return s;
}


void omeSceneDestroy(omeScene **s)
{
    // TODO: free geometries here???
    memset(*s, 0, sizeof(omeScene));
    free(*s);
    *s = NULL;
}


void omeSceneAddGeometry(omeScene *s, omeGeometry *g)
{
    omeGeometryListElement *element = calloc(1, sizeof(omeGeometryListElement));
    element->geometry = g;

    DL_APPEND(s->geometries, element);
}


void omeSceneRender(omeScene *s, omeCamera *c)
{
    omeGeometryListElement *geometryElt;
    
    DL_FOREACH(s->geometries, geometryElt)
    {
        omeMeshListElement *meshElt;
        omeGeometry *g = geometryElt->geometry;

        omeGeometrySendAttributes(g);

        DL_FOREACH(g->references, meshElt)
        {         
            omeProgram *p = meshElt->mesh->program;
            omeMaterial *m = meshElt->mesh->material;

            // TODO: When the code will be cleaner, remove this. This engine is meant to be shader only
            if(p)
            {
                omeProgramUse(p);

                // TODO: same here I guess... (default material set in meshCreate() maybe?)
                if(m)
                    omeProgramSendUniformMaterial(p, m, "mat");
            }

            omeGeometryRender(g);
        }

        omeGeometryDisableAttributes(g);
    }
}
