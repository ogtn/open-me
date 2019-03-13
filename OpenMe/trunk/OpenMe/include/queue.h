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


// FIFO queue, with fixed size
typedef struct omeQueue 
{
    int size;			//
    int head;			// head index
    int tail;			// tail index
    int current;		// index of current element (omeQueueForEach)
    int nbElements;		// number of elements
    void **elements;	// element array (always contains at least one empty space)
} omeQueue;


// Constructor and destructor of a queue. Size is the max number of elements that will be stored
omeQueue *omeQueueCreate(int size);
void omeQueueDestroy(omeQueue **q);

// Check if the queue is full or empty
omeBool omeQueueIsEmpty(omeQueue *q);
omeBool omeQueueIsFull(omeQueue *q);

// add and remove an element, return OME_SUCCESS if it succeed
omeStatus omeQueuePush(omeQueue *q, void *element);
omeStatus omeQueuePop(omeQueue *q, void **res);

// return the number of elements currently stored
int omeQueueGetNbElements(omeQueue *q);

// iterate over queue elements, take an omeQueue *, an int, and a pointer to an element
#define omeQueueForEach(q, itr, elt) 									\
	for((itr) = (q)->head, (elt) = (q)->elements[(q)->head];			\
		(itr) != (q)->tail;												\
		(itr) = ((itr) + 1) % (q)->size, (elt) = (q)->elements[(itr)])


#ifdef __cplusplus
}
#endif


#endif // OME_QUEUE_H
