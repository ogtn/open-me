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
    omeProgram *p =  NULL;

    omeCameraUpdate(c);

    DL_FOREACH(s->geometries, geometryElt)
    {
        omeMeshListElement *meshElt;
        omeGeometry *g = geometryElt->geometry;    

        DL_FOREACH(g->references, meshElt)
        {         
            omeMaterial *m = meshElt->mesh->material;

            if(p != meshElt->mesh->program)
            {
                p = meshElt->mesh->program;

                omeProgramUse(p);
                omeGeometryEnableAttributes(g, p);
                omeGeometrySendAttributes(g);
            }

            // uniforms
            omeProgramSendUniformCamera(p, c);
            omeProgramSendUniformMaterial(p, m, "mat");
            omeProgramSendUniformEntity(p, meshElt->mesh);
            omeProgramSendUniformVec(p, &c->pos, "omeCameraPosition");

            omeGeometryRender(g);

            //omeGeometryDisableAttributes(g);
        }

        p = NULL;
    }
}
