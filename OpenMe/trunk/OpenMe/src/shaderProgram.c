//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "shaderProgram.h"
#include "material.h"
#include "logger.h"
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


omeShader *omeShaderCreate(char *fileName)
{
    omeShader *s;
    FILE *file;
    char *ext;
    int size;
    char c;
    int i;

    ext = strrchr(fileName, '.');

    if(ext == NULL)
    {
        omeLoggerLog("Unable to determine shader type: no file extension in \"%s\"!\n", fileName);
        return NULL;
    }

    file = fopen(fileName, "r");

    if(file == NULL)
        return NULL;

    s = calloc(1, sizeof(omeShader));

    // loading shader code
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    s->code = malloc(size + 1);

    for(i = 0; i < size; i++)
    {
        c = (char)fgetc(file);
        s->code[i] = c == EOF ? '\0' : c; // fuckin' lame old drivers
    }

    s->code[size] = '\0';
    fclose(file);

    s->compiled = OME_FALSE;

    // create OpenGl shader from file extension
    if(!strcmp(ext, ".vs"))
    {
        s->id = glCreateShader(GL_VERTEX_SHADER);
        s->type = OME_SHADER_TYPE_VERTEX;
    }
    else if(!strcmp(ext, ".ps"))
    {
        s->id = glCreateShader(GL_FRAGMENT_SHADER);
        s->type = OME_SHADER_TYPE_PIXEL;
    }
    else
    {
        omeLoggerLog("Unable to determine shader type: extension \"%s\" unknown!\n", ext);
        return NULL;
    }

    return s;
}


void omeShaderDestroy(omeShader **s)
{
    glDeleteShader((*s)->id);
    free((*s)->code);
    memset(*s, 0, sizeof(omeShader));
    free(*s);
    *s = NULL;
}


omeStatus omeShaderCompile(omeShader *s)
{
    GLint status;
    GLsizei size;
    char log[OME_PROGRAM_LOG_LENGTH]; // TODO use glGetShader with GL_INFO_LOG_LENGTH instead?

    // send source code
    glShaderSource(s->id, 1, &s->code, NULL);

    // compiling and testing
    glCompileShader(s->id);
    glGetShaderiv(s->id, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE)
    {
        glGetShaderInfoLog(s->id, OME_PROGRAM_LOG_LENGTH, &size, log);

        omeLoggerLog("Error while compiling shader\n");        
        omeLoggerLog("%s\n", log);

        return OME_FAILURE;
    }

    s->compiled = OME_TRUE;

    return OME_SUCCESS;
}


omeProgram *omeProgramCreate(void)
{
    omeProgram *sp;

    sp = calloc(1, sizeof(omeProgram));
    sp->id = glCreateProgram();
    sp->linked = OME_FALSE;
    sp->status = OME_PROGRAM_STATUS_NOT_READY;

    return sp;
}


void omeProgramDestroy(omeProgram **sp)
{
    int i;
    omeLocation *loc, *tmp;

    for(i = 0; i < OME_SHADER_TYPE_MAX; i++)
    {
        if((*sp)->shaders[i])
        {
            glDetachShader((*sp)->id, (*sp)->shaders[i]->id);
            omeShaderDestroy(&(*sp)->shaders[i]);
        }
    }

    // free hashtable and locations
    HASH_ITER(hh, (*sp)->uniforms, loc, tmp) 
    {
        HASH_DEL((*sp)->uniforms, loc);
        omeLocationDestroy(&loc);
    }

    glDeleteProgram((*sp)->id);
    memset(*sp, 0, sizeof(omeProgram));
    *sp = NULL;
}


void omeProgramAddShader(omeProgram *sp, omeShader *s)
{
    if(sp->shaders[s->type])
    {
        glDetachShader(sp->id, sp->shaders[s->type]->id);
        omeShaderDestroy(&sp->shaders[s->type]);
    }

    sp->shaders[s->type] = s;
    sp->linked = OME_FALSE;
    sp->status = OME_PROGRAM_STATUS_NOT_READY;
    sp->shadersAttached[s->type] = OME_FALSE;
}


omeStatus omeProgramPrepareLink(omeProgram *sp)
{
    int i;

    // check that there are at least both pixel and a vertex shaders...
    /*   if(sp->shaders[OME_SHADER_TYPE_PIXEL] == NULL
    || sp->shaders[OME_SHADER_TYPE_VERTEX] == NULL)
    goto error;
    */
    // compile and attach all shaders...
    for(i = 0; i < OME_SHADER_TYPE_MAX; i++)
    {
        if(sp->shaders[i] != NULL)
        {
            if(sp->shaders[i]->compiled == OME_FALSE && omeShaderCompile(sp->shaders[i]) == OME_FAILURE)
                goto error;

            if(sp->shadersAttached[i] == OME_FALSE)
            {
                glAttachShader(sp->id, sp->shaders[i]->id);
                sp->shadersAttached[i] = OME_TRUE;
            }
        }
    }

    sp->status = OME_PROGRAM_STATUS_READY;
    return OME_SUCCESS;

error:
    sp->status = OME_PROGRAM_STATUS_BROKEN;
    return OME_FAILURE;
}


omeStatus omeProgramLink(omeProgram *sp)
{
    GLint status;
    GLsizei size;
    char log[OME_PROGRAM_LOG_LENGTH]; // TODO use glGetProgram with GL_INFO_LOG_LENGTH instead?

    switch(sp->status)
    {
    case OME_PROGRAM_STATUS_READY:
        break;
    case OME_PROGRAM_STATUS_NOT_READY:
        omeProgramPrepareLink(sp);
        break;
    case OME_PROGRAM_STATUS_BROKEN:
        return OME_FAILURE;
    default:
        omeLoggerLog("Invalid status in shader program\n");
        return OME_FAILURE;
    }

    // linking
    glLinkProgram(sp->id);
    glGetProgramiv(sp->id, GL_LINK_STATUS, &status);

    if(status == GL_FALSE)
    {
        glGetProgramInfoLog(sp->id, OME_PROGRAM_LOG_LENGTH, &size, log);

        omeLoggerLog("Error while linking shader program\n");
        omeLoggerLog("%s\n", log);

        return OME_FAILURE;
    }

    // fill the uniforms cache
    // TODO: add a boolean value to tell if the cache is valid or to be deleted?
    omeProgramLocateUniforms(sp);
    omeProgramLocateAttributes(sp);

    return OME_SUCCESS;
}


void omeProgramUse(omeProgram *sp)
{
    glUseProgram(sp->id);
}


void omeProgramLocateUniforms(omeProgram *sp)
{
    int i;
    int len;
    int nbUniforms;
    char name[64];
    GLint size;
    GLenum type;

    glGetProgramiv(sp->id, GL_ACTIVE_UNIFORMS, &nbUniforms);

    for(i = 0; i < nbUniforms; i++)
    {
        glGetActiveUniform(sp->id, i, 64, &len, &size, &type, name);
        omeProgramLocateUniform(sp, name);
    }
}


void omeProgramLocateAttributes(omeProgram *sp)
{
    int i;
    int len;
    int nbAttributes;
    char name[64];
    GLint size;
    GLenum type;

    glGetProgramiv(sp->id, GL_ACTIVE_ATTRIBUTES, &nbAttributes);

    for(i = 0; i < nbAttributes; i++)
    {
        glGetActiveAttrib(sp->id, i, 64, &len, &size, &type, name);
        printf("attribute %d: %s located at %d\n", i, name, glGetAttribLocation(sp->id, name));
    }
}


int omeProgramLocateUniform(omeProgram *sp, char *name)
{
    omeLocation *loc;
    
    HASH_FIND_STR(sp->uniforms, name, loc);

    if(loc)
        return loc->location;
    else
    {
        loc = omeUniformLocationCreate(sp, name);

        if(loc != NULL)
        {
            HASH_ADD_STR(sp->uniforms, key, loc);
            return loc->location;
        }
    }

    return -1;
}


omeLocation *omeLocationCreate(omeProgram *sp, char *name, omeLocationType type)
{
    omeLocation *loc = calloc(1, sizeof(omeLocation));

    switch(type)
    {
    case OME_LOCATION_TYPE_UNIFORM:
        loc->location = glGetUniformLocation(sp->id, name);
        break;
    case OME_LOCATION_TYPE_ATTRIB:
        loc->location = glGetAttribLocation(sp->id, name);
        break;
    default:
        omeLoggerLog("You shouldn't be here\n");
        return NULL;
    }

    //TODO: this function sucks, not garanted to be null terminated and can't be sure that the whole string was copied...
    strncpy(loc->key, name, OME_PROGRAM_VAR_LENGTH);
    loc->key[OME_PROGRAM_VAR_LENGTH - 1];
    
    if(strcmp(loc->key, name))
    {
        omeLoggerLog("Problem with location: name is too long\n");
        free(loc);
        
        return NULL;
    }

    return loc;
}


void omeLocationDestroy(omeLocation **loc)
{
    memset(*loc, 0, sizeof(omeLocation));
    free(*loc);
    *loc = NULL;
}


void omeProgramSendUniformf(omeProgram *sp, float f, char *name)
{
    omeProgramUse(sp);
    glUniform1f(omeProgramLocateUniform(sp, name), f);
}


void omeProgramSendUniformMaterial(omeProgram *p, omeMaterial *m, char *name)
{
    char fullName[OME_PROGRAM_VAR_LENGTH] = {'\0'};

    sprintf(fullName, "%s.ambiant", name);
    glUniform3fv(omeProgramLocateUniform(p, fullName), 1, m->ambiantColor.tab); // /!\ omeColorf is made of 4 floats for the moment, only three are send here
}
