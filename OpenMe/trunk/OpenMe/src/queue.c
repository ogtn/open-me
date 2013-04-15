//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "queue.h"
#include <stdio.h>


omeQueue *omeQueueCreate(int size)
{
	omeQueue *q = calloc(1, sizeof(omeQueue));

	q->size = size;
	q->head = 0;
	q->tail = size - 1;
	q->nbElements = 0;
	q->elements = calloc(size, sizeof(void *));

	return q;
}


void omeQueueDestroy(omeQueue **q)
{
	free((*q)->elements);
	free(*q);
	*q = NULL;
}


omeBool omeQueueIsEmpty(omeQueue *q)
{
	return q->nbElements > 0 ? OME_FALSE : OME_TRUE;
}


omeBool omeQueueIsFull(omeQueue *q)
{
	return q->nbElements < q->size ? OME_FALSE : OME_TRUE;
}


omeStatus omeQueuePush(omeQueue *q, void *element)
{
	if(omeQueueIsFull(q))
		return OME_FAILURE;

	q->nbElements++;
	q->tail = (q->tail + q->size + 1) % q->size;
	q->elements[q->tail] = element;

	return OME_SUCCESS;
}


omeStatus omeQueuePop(omeQueue *q, void **res)
{
	if(omeQueueIsEmpty(q))
	{
		*res = NULL;
		return OME_FAILURE;
	}

	q->nbElements--;
	*res = q->elements[q->head];
	q->elements[q->head] = NULL;
	q->head = (q->head + q->size + 1) % q->size;

	return OME_SUCCESS;
}


int omeQueueGetNbElements(omeQueue *q)
{
	return q->nbElements;
}
