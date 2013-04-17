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
#include <unistd.h>
#include <stdio.h>


#define NB_THREADS		16
#define NB_TASKS		4096


int tough_work(int n)
{
	int i;

	for(i = 0; i < 500000; i++)
		n *= n * n;

	return n;
}


void task1(void *context, void *arg)
{
	int *id = arg;
	int *results = context;

	results[*id] = *id + 100;
	results[NB_TASKS] = tough_work(results[NB_TASKS]);
}


int main(void)
{
	int i, id;
	omeThreadPool *pool = NULL;
	int args[NB_TASKS];
	int results[NB_TASKS + 1];
	
	pool = omeThreadPoolCreate(NB_THREADS, NB_TASKS, results);
	assert(pool != NULL);
	assert(omeThreadPoolGetRunningTheads(pool) == 0);
	assert(omeThreadPoolGetRemainingTasks(pool) == 0);
	assert(omeThreadPoolPause(pool) == OME_SUCCESS);

	for(i = 0; i < NB_TASKS; i++)
	{
		args[i] = i;
		results[i] = 0;
		assert(omeThreadPoolAddTask(pool, task1, &args[i], &id) == OME_SUCCESS);
	}

	assert(omeThreadPoolGetRemainingTasks(pool) == NB_TASKS);
	sleep(1);
	assert(omeThreadPoolFlush(pool) == OME_FAILURE);
	assert(omeThreadPoolGetRemainingTasks(pool) == NB_TASKS);
	assert(omeThreadPoolUnPause(pool) == OME_SUCCESS);

	assert(omeThreadPoolFlush(pool) == OME_SUCCESS);
	assert(omeThreadPoolGetRunningTheads(pool) == 0);
	assert(omeThreadPoolGetRemainingTasks(pool) == 0);

	for(i = 0; i < NB_TASKS; i++)
	{
		args[i] = i;
		results[i] = 0;
		assert(omeThreadPoolAddTask(pool, task1, &args[i], &id) == OME_SUCCESS);
	}

	assert(omeThreadPoolFlush(pool) == OME_SUCCESS);
	assert(omeThreadPoolGetRunningTheads(pool) == 0);
	assert(omeThreadPoolGetRemainingTasks(pool) == 0);
	omeThreadPoolDestroy(&pool);
	assert(pool == NULL);

	for(i = 0; i < NB_TASKS; i++)
		assert(results[i] == (i + 100));

	return EXIT_SUCCESS;
}
