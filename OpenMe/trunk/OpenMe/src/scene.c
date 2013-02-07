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
#include "program.h"
#include "engine.h"
#include "logger.h"
#include <GL/glew.h>
#include <utlist.h>
#include <stdlib.h>
#include <string.h>


#include "light.h"
omeScene *omeSceneCreate(void)
{
    omeLight *light;
    omeVector lightPos = {{15, 15, 15}};
    omeScene *s = calloc(1, sizeof(omeScene));

    light = omeLightCreateOmni();
    light->entity.position = lightPos;
    omeSceneAddLight(s, light);

    return s;
}


void omeSceneDestroy(omeScene **s)
{
    // TODO: free geometries and lights here???
    omeDbgClearMem(*s, sizeof(omeScene));
    free(*s);
    *s = NULL;
}


void omeSceneAddGeometry(omeScene *s, omeGeometry *g)
{
    omeGeometryListElement *element = calloc(1, sizeof(omeGeometryListElement));
    element->geometry = g;

    DL_APPEND(s->geometries, element);
}


void omeSceneAddLight(omeScene *s, omeLight *l)
{
    // TODO: change to something more efficient than a realloc for each call
    s->nbLights++;
    s->lights = realloc(s->lights, s->nbLights * sizeof(omeLight *));
    s->lights[s->nbLights - 1] = l;
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
            omeMesh *mesh = meshElt->mesh;

            // attributes
            if(p != mesh->program)
            {
                p = mesh->program;

                omeProgramUse(p);
                omeGeometryEnableAttributes(g, p);
                omeGeometrySendAttributes(g);
            }

            // uniforms
            omeProgramSendUniformCamera(p, c);
            
            if(mesh->material)
                omeProgramSendUniformMaterial(p, mesh->material, "mat");
            
            omeProgramSendUniformEntity(p, mesh);
            omeProgramSendUniformVec(p, &c->pos, "omeCameraPosition");
            omeProgramSendUniformLight(p, s->lights[0], "light0");

            omeGeometryRender(g);

            //omeGeometryDisableAttributes(g);
        }

        p = NULL;
    }
}


omeMesh *omeScenePick(omeScene *s, omeCamera *c, int x, int y)
{
    omeGeometryListElement *geometryElt;
    omeProgram *p = omeEngineGetPickingProgram();
    // TODO: allow more than 255 meshes with the use of the other components
    unsigned char id = 0;
    unsigned char pixel;

    omeCameraUpdate(c);
    omeProgramUse(p);

    DL_FOREACH(s->geometries, geometryElt)
    {
        omeMeshListElement *meshElt;
        omeGeometry *g = geometryElt->geometry;    

        // TODO: exclude non candidate meshes?
        DL_FOREACH(g->references, meshElt)
        {         
            // attributes
            // TODO: only enable positions for picking...
            omeGeometryEnableAttributes(g, p);
            omeGeometrySendAttributes(g);

            // uniforms
            omeProgramSendUniformCamera(p, c);
            omeProgramSendUniformEntity(p, meshElt->mesh);
            omeProgramSendUniformVec(p, &c->pos, "omeCameraPosition");
            omeProgramSendUniformf(p, id / 256.f, "omeId");

            omeGeometryRender(g);
            id++;
        }
    }

    glReadPixels(x, y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &pixel);

    // nothing found
    if(pixel >= id)
        return NULL;

    id = 0;

    DL_FOREACH(s->geometries, geometryElt)
    {
        omeMeshListElement *meshElt;
        omeGeometry *g = geometryElt->geometry;    

        DL_FOREACH(g->references, meshElt)
        {
            if(id++ == pixel)
                return meshElt->mesh;
        }
    }

    // shouldn't happen
    omeLoggerLog("Shit happened while picking\n");

    return NULL;
}
