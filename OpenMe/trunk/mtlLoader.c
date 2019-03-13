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
#include "material.h"
#include <stdio.h>


omePluginType omePluginInit(void)
{
	return OME_PLUGIN_TYPE_MATERIAL_LOADER;
}


void omePluginQuit(void)
{
}


omeMaterial *loadFromFile(const char *fileName)
{
    omeMaterial *m = NULL;
    FILE *file;
    char line[1024];

    file = fopen(fileName, "r");

    if(!file)
        return NULL;

    while(fgets(line, 1024, file))
    {
        omeCleanString(line);

        // new material
        if(strstr(line, "newmtl ") == line)
        {
            if(m)
            {
                // register material in resource manager here
                m = NULL;
            }

            m = omeMaterialCreate();
            omeResourceSetName(line + strlen("newmtl "));
        }
        // ambiant reflectivity
        else if(strstr(line, "Ka"))
        {
            float r, g, b;

            if(sscanf(line, "Ka %f %f %f", &r, &g, &b) != 3)
                b = g = r;
        }
        // diffuse reflectivity
        else if(strstr(line, "Kd")){ /* same as above */ }
        // specular reflectivity
        else if(strstr(line, "Ks ")){ /* same as above */ }
        // transmission filter
        else if(strstr(line, "Tf ")){}
        // illumination model
        else if(strstr(line, "illum "))
        {
            int model;

            sscanf(line, "illum %d", &model);

            /*
             * 0    Color on and Ambient off 
             * 1    Color on and Ambient on 
             * 2    Highlight on 
             * 3    Reflection on and Ray trace on 
             * 4    Transparency: Glass on 
             * Reflection: Ray trace on 
             * 5    Reflection: Fresnel on and Ray trace on 
             * 6    Transparency: Refraction on 
             * Reflection: Fresnel off and Ray trace on 
             * 7    Transparency: Refraction on 
             * Reflection: Fresnel on and Ray trace on 
             * 8    Reflection on and Ray trace off 
             * 9    Transparency: Glass on 
             * Reflection: Ray trace off 
             * 10   Casts shadows onto invisible surfaces 
             */
        }
        else if(strstr(line, "d -halo ")){}
        // specular exponent [0-1000]
        else if(strstr(line, "Ns ")){}
        // sharpness [0-1000], default 60
        else if(strstr(line, "sharpness ")){}
        // index of refraction [0.001-10]
        else if(strstr(line, "Ni ")){}
        // texture map
        else if(strstr(line, "map_Ka ")){}

        else if(strstr(line, "map_Kd ")){}
        else if(strstr(line, "map_Ks ")){}
        else if(strstr(line, "map_Ns ")){}
        else if(strstr(line, "map_d ")){}
        else if(strstr(line, "disp ")){}
        else if(strstr(line, "decal ")){}
        else if(strstr(line, "bump ")){}
        // reflection
        else if(strstr(line, "refl ")){}
    }

    fclose(file);

    return m; //TODO: wouldnt a boolean be better than a pointer? let that job to the resource manager...
}


omeBool parseRGB(const char *line, const char *param, float *r, float *g, float *b)
{
    char format[128];

    sprintf(format, "%s %%s %%s %%s", param);

    if(strstr(param, line))
    {
        switch(sscanf(line, format, r, g, b))
        {
            case 1:
                b = g = r;
                break;
            case 2:
                b = r;
                break;
        }

        return OME_TRUE;
    }
    else
        return OME_FALSE;
}

