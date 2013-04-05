//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "shader.h"
#include "logger.h"
#include "opengl.h"
#include <stdlib.h>


omeShader *omeShaderCreate(omeShaderType type, char *code, const char *name)
{
    omeShader *s = calloc(1, sizeof(omeShader));

    switch(type)
    {
    case OME_SHADER_TYPE_PIXEL:
        omeResourceInitialize(&s->resource, OME_RESOURCE_PIXEL_SHADER, name);
        s->id = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case OME_SHADER_TYPE_VERTEX:
        omeResourceInitialize(&s->resource, OME_RESOURCE_VERTEX_SHADER, name);
        s->id = glCreateShader(GL_VERTEX_SHADER);
        break;
    case OME_SHADER_TYPE_MAX:
    default:
        omeLoggerLog("Shader type is not valid\n");
        free(s);
        return NULL;
    }

    s->code = code;
    s->type = type;
    s->compiled = OME_FALSE;

    return s;
}


omeShader *omeShaderLoadFromFile(const char *fileName)
{
    omeShaderType type;
    omeShader *s;
    FILE *file;
    char *code;
    char *ext;
    int size;
    char c;
    int i;

    // shader already loaded
    s = omeResourceFind(fileName);

    if(s != NULL)
    {
        omeResourceAddRef(&s->resource);
        return s;
    }

    // look for a file extension
    ext = strrchr(fileName, '.');

    if(ext == NULL)
    {
        omeLoggerLog("Unable to determine shader type: no file extension in \"%s\"!\n", fileName);
        return NULL;
    }

    // determine shader type using the extension
    if(!strcmp(ext, ".vs"))
        type = OME_SHADER_TYPE_VERTEX;
    else if(!strcmp(ext, ".ps"))
        type = OME_SHADER_TYPE_PIXEL;
    else
    {
        omeLoggerLog("Unable to determine shader type: extension \"%s\" unknown!\n", ext);
        return NULL;
    }

    file = fopen(fileName, "r");

    if(file == NULL)
    {
        omeLoggerLog("Unable to open file %s\n", fileName);
        return NULL;
    }

    // loading shader code
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);
    code = malloc(size + 1);

    for(i = 0; i < size; i++)
    {
        c = (char)fgetc(file);
        code[i] = c == EOF ? '\0' : c; // fuckin' lame old drivers
    }

    code[size] = '\0';
    fclose(file);

    return omeShaderCreate(type, code, fileName);
}


void omeShaderDestroy(omeShader **s)
{
    glDeleteShader((*s)->id);
    free((*s)->code);
    omeDbgClearMem(*s, sizeof(omeShader));
    free(*s);
    *s = NULL;
}


omeStatus omeShaderCompile(omeShader *s)
{
    GLint status;
    GLsizei size;

    if(s->compiled == OME_TRUE)
    {
        

        return OME_SUCCESS;
    }

    // send source code
    glShaderSource(s->id, 1, (const char **)&s->code, NULL);

    // compiling and testing
    glCompileShader(s->id);
    glGetShaderiv(s->id, GL_COMPILE_STATUS, &status);

    if(status == GL_FALSE)
    {
        int logSize;
        char *log;
        
        glGetShaderiv(s->id, GL_INFO_LOG_LENGTH, &logSize);
        log = malloc(logSize);
        
        glGetShaderInfoLog(s->id, logSize, &size, log);

        omeLoggerLog("Error while compiling shader:\n%s\n", log);        
        free(log);

        return OME_FAILURE;
    }

    s->compiled = OME_TRUE;

    return OME_SUCCESS;
}
