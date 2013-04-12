//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "threadPool.h"
#include <assert.h>


#define NB_THREADS		10
#define NB_TASKS		4


void task1(void *context, void *arg)
{
	int *id = arg;
	int *results = context;

	results[*id] = *id + 100;

	sleep(1);
}


int main(void)
{
	int i, id;
	omeThreadPool *pool;
	int args[NB_TASKS];
	int results[NB_TASKS];

	for(i = 0; i < NB_TASKS; i++)
	{
		args[i] = i;
		results[i] = 0;
	}

	pool = omeThreadPoolCreate(NB_THREADS, NB_TASKS, results);

	for(i = 0; i < NB_TASKS; i++)
		assert(omeThreadPoolAddTask(pool, task1, &args[i], &id) == OME_SUCCESS);

	sleep(3);
	omeThreadPoolDestroy(&pool);
	assert(pool == NULL);

	for(i = 0; i < NB_TASKS; i++)
		assert(results[i] == (i + 100));

	return EXIT_SUCCESS;
}
