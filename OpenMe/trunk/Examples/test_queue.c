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
#include <assert.h>
#include <stdlib.h>


int main(void)
{
	void *res;
	int i;
	int values[10];
	omeQueue *q = omeQueueCreate(10);

	// check if new queue is really clean
	assert(omeQueueIsEmpty(q) == OME_TRUE);
	assert(omeQueueIsFull(q) == OME_FALSE);
	assert(omeQueuePop(q, &res) == OME_FAILURE);

	// fill it to the max
	for(i = 0; i < 10; i++)
	{
		values[i] = i;
		assert(omeQueuePush(q, &values[i]) == OME_SUCCESS);
	}

	// check if it is really full
	assert(omeQueueIsFull(q) == OME_TRUE);
	assert(omeQueueIsEmpty(q) == OME_FALSE);

	// check if the content is ok
	for(i = 0; i < 10; i++)
	{
		assert(omeQueuePop(q, &res) == OME_SUCCESS);
		assert(*((int *)res) == i);
	}

	// check if emptied queue is really clean
	assert(omeQueueIsEmpty(q) == OME_TRUE);
	assert(omeQueueIsFull(q) == OME_FALSE);
	assert(omeQueuePop(q, &res) == OME_FAILURE);

	omeQueueDestroy(&q);
	assert(q == NULL);

	return EXIT_SUCCESS;
}
