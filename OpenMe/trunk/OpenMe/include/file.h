//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_PATH_H
#define OME_PATH_H


#ifdef __cplusplus
extern "C" {
#endif


#include "dstring.h"
#include <stdio.h>


// default allocated size for paths
#define OME_FILE_NAME_MAXLEN            256

// flags for file opening
#define OME_FILE_START
#define OME_FILE_END
#define OME_FILE_READ
#define OME_FILE_WRITE
#define OME_FILE_RW
#define OME_FILE_BIN


typedef enum omeFileStatus
{
    OME_FILE_STATUS_OPENED,
    OME_FILE_STATUS_CLOSED
} omeFileStatus;


typedef struct omeFile
{
    omeString *name;
    omeFileStatus status;
    FILE *file;
} omeFile;


omeFile *omeFileCreate(const char *path);
void omeFileDestroy(omeFile **f);

int omeFileIsValid(omeFile *f);
int omeFileIsFile(omeFile *f);
int omeFileIsDir(omeFile *f);

void omeFileOpen(omeFile *f);
void omeFileClose(omeFile *f);

void omeFileGotoParent(omeFile *f);
void omeFileOpenChild(omeFile *f, int child);
//???? omeFileGetContent(omeFile *f);


#ifdef __cplusplus
}
#endif


#endif // OME_PATH_H

