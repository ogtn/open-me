//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_QUEUE_H
#define OME_QUEUE_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"


typedef struct omeQueue
{
    int size;
    int head;
    int tail;
    int nbElements;
    void **elements;
} omeQueue;


omeQueue *omeQueueCreate(int size);
void omeQueueDestroy(omeQueue **q);
omeBool omeQueueIsEmpty(omeQueue *q);
omeBool omeQueueIsFull(omeQueue *q);
omeStatus omeQueuePush(omeQueue *q, void *element);
omeStatus omeQueuePop(omeQueue *q, void **res);


#ifdef __cplusplus
}
#endif


#endif // OME_QUEUE_H
