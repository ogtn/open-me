//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "shaderProgram.h"
#include "logger.h"
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
    char log[OME_SHADER_LOG_LENGTH]; // TODO use glGetShader with GL_INFO_LOG_LENGTH instead?

    // send source code
    glShaderSource(s->id, 1, &s->code, NULL);

    // compiling and testing
    glCompileShader(s->id);
    glGetShaderiv(s->id, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE)
    {
        glGetShaderInfoLog(s->id, OME_SHADER_LOG_LENGTH, &size, log);

        omeLoggerLog("Error while compiling shader\n");        
        omeLoggerLog("%s\n", log);

        return OME_FAILURE;
    }

    s->compiled = OME_TRUE;

    return OME_SUCCESS;
}


omeShaderProgram *omeShaderProgramCreate(void)
{
    omeShaderProgram *sp;

    sp = calloc(1, sizeof(omeShaderProgram));
    sp->id = glCreateProgram();
    sp->linked = OME_FALSE;
    sp->status = OME_SHADER_PROGRAM_STATUS_NOT_READY;

    return sp;
}


void omeShaderProgramDestroy(omeShaderProgram **sp)
{
    int i;

    for(i = 0; i < OME_SHADER_TYPE_MAX; i++)
    {
        if((*sp)->shaders[i])
        {
            glDetachShader((*sp)->id, (*sp)->shaders[i]->id);
            omeShaderDestroy(&(*sp)->shaders[i]);
        }
    }

    glDeleteProgram((*sp)->id);
    memset(*sp, 0, sizeof(omeShaderProgram));
    *sp = NULL;
}


void omeShaderProgramAddShader(omeShaderProgram *sp, omeShader *s)
{
    if(sp->shaders[s->type])
    {
        glDetachShader(sp->id, sp->shaders[s->type]->id);
        omeShaderDestroy(&sp->shaders[s->type]);
    }

    sp->shaders[s->type] = s;
    sp->linked = OME_FALSE;
    sp->status = OME_SHADER_PROGRAM_STATUS_NOT_READY;
    sp->shadersAttached[s->type] = OME_FALSE;
}


omeStatus omeShaderProgramPrepareLink(omeShaderProgram *sp)
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

    sp->status = OME_SHADER_PROGRAM_STATUS_READY;
    return OME_SUCCESS;

error:
    sp->status = OME_SHADER_PROGRAM_STATUS_BROKEN;
    return OME_FAILURE;
}


omeStatus omeShaderProgramLink(omeShaderProgram *sp)
{
    GLint status;
    GLsizei size;
    char log[OME_SHADER_LOG_LENGTH]; // TODO use glGetProgram with GL_INFO_LOG_LENGTH instead?
    
    switch(sp->status)
    {
    case OME_SHADER_PROGRAM_STATUS_READY:
        break;
    case OME_SHADER_PROGRAM_STATUS_NOT_READY:
        omeShaderProgramPrepareLink(sp);
        break;
    case OME_SHADER_PROGRAM_STATUS_BROKEN:
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
        glGetProgramInfoLog(sp->id, OME_SHADER_LOG_LENGTH, &size, log);

        omeLoggerLog("Error while linking shaderr program\n");
        omeLoggerLog("%s\n", log);

        return OME_FAILURE;
    }


    // crappy test, seams interesting for generating the uniform location cache...
    {
        int i;
        int size;
        int nbUniforms;
        char name[64];
        int dummy;

        glGetProgramiv(sp->id, GL_ACTIVE_UNIFORMS, &nbUniforms);
        
        for(i = 0; i < nbUniforms; i++)
        {
            glGetActiveUniform(sp->id, i, 64, &size, &dummy, &dummy, name);
            printf("unform %d: %s\n", i, name);
        }
    }

    return OME_SUCCESS;
}
