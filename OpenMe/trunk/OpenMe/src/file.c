//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "file.h"
#include <stdlib.h>


omeFile *omeFileCreate(const char *path)
{
    omeFile *f = malloc(sizeof(omeFile));

    f->file = NULL;
    f->status = OME_FILE_STATUS_CLOSED;

    if(path != NULL)
        f->name = omeStringCreate(OME_FILE_NAME_MAXLEN);
    else
        f->name = omeStringCreateFromStr(path);

    return f;
}


void omeFileDestroy(omeFile **f)
{
    omeStringDestroy(&((*f)->name));

    if((*f)->status != OME_FILE_STATUS_CLOSED)
        fclose((*f)->file);
    
    *f = NULL;
}


// int omeFileIsValid(omeFile *f)
// {
// }


// int omeFileIsFile(omeFile *f)
// {
// }


// int omeFileIsDir(omeFile *f)
// {
// }



// void omeFileOpen(omeFile *f)
// {
// }


// void omeFileClose(omeFile *f)
// {
// }



// void omeFileGotoParent(omeFile *f)
// {
// }


// void omeFileOpenChild(omeFile *f, int child)
// {
// }

/* TODO: implement this
???? omeFileGetContent(omeFile *f)
{
}
*/
