//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_MATERIAL_H
#define OME_MATERIAL_H


#ifdef __cplusplus
extern "C" {
#endif


typedef struct omeTexture omeTexture;


typedef union omeColori
{
    struct
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };

    unsigned char tab[4];
} omeColori;


typedef union omeColorf
{
    struct
    {
        float r;
        float g;
        float b;
        float a;
    };

    float tab[4];
} omeColorf;


typedef struct omeMaterial
{
    omeColorf ambiantColor;
    omeColorf diffuseColor;
    omeColorf specularColor;
    omeColorf emissiveColor;
    float shininess;

    // TODO: find a place for this one...?
    //omeTexture *ambiantTexture;
    omeTexture *diffuseTexture;
    omeTexture *specularTexture;
    omeTexture *emissiveTexture;
    omeTexture *shininessTexture;
} omeMaterial;


omeMaterial *omeMaterialCreate(void);
void omeMaterialDestroy(omeMaterial **m);


#ifdef __cplusplus
}
#endif


#endif // OME_MATERIAL_H
