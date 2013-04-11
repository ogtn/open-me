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


omeTask *omeTaskCreate(omeThreadPoolProcessTask func, void *arg, int id)
{
    omeTask *t = calloc(1, sizeof(omeTask));

    t->func = func;
    t->arg = arg;
    t->id = id;

    return t;
}


void omeTaskDestroy(omeTask **t)
{
    free(*t);
    *t = NULL;
}


omeThreadPool *omeThreadPoolCreate(int maxThreads, int maxTasks, void *context)
{
    int i;
    omeThreadPool *tp;
    pthread_attr_t thread_attr;
    pthread_condattr_t cond_attr;
    pthread_mutexattr_t mutex_attr;
     
    // thread pool
    tp = calloc(1, sizeof(omeThreadPool));
    tp->maxThreads = maxThreads;
    tp->maxTasks = maxTasks;
    tp->context = context;
    tp->lastId = -1;
    tp->lastStarted = -1;
    tp->waitingTasks = omeQueueCreate(maxTasks);
    tp->terminated = OME_FAILURE;

    // running threads
    tp->running = calloc(maxThreads, sizeof(int));

    for(i = 0; i < maxThreads; i++)
        tp->running[i] = -1;

    // mutex
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&tp->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    // cond
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&tp->cond, &cond_attr);
    pthread_condattr_destroy(&cond_attr);

    // threads
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < maxThreads; i++)
        pthread_create(&tp->threads[i], &thread_attr, omeThreadPoolMain, tp);

    pthread_attr_destroy(&thread_attr);

    return tp;
}


void omeThreadPoolDestroy(omeThreadPool **tp)
{
    int i;
    void *retVal;

    for(i = 0; i < (*tp)->maxThreads; i++)
    {
        pthread_cancel((*tp)->threads[i]);
        pthread_join((*tp)->threads[i], &retVal);
    }

    pthread_cond_destroy(&(*tp)->cond);
    pthread_mutex_destroy(&(*tp)->mutex);
    omeQueueDestroy(&(*tp)->waitingTasks);
    free((*tp)->running);
    free(*tp);
    *tp = NULL;
}


omeStatus omeThreadPoolAddTask(omeThreadPool *tp, omeThreadPoolProcessTask func, void *arg, int *taskId)
{
    omeTask *t;
    omeStatus status;

    *taskId = -1;

    pthread_mutex_lock(&tp->mutex);

    t = omeTaskCreate(func, arg, tp->lastId + 1);
    status = omeQueuePush(tp->waitingTasks, t);

    if(status == OME_SUCCESS)
    {
        tp->lastId++;
        *taskId = tp->lastId;
    }
    else
        omeTaskDestroy(&t);

    pthread_mutex_unlock(&tp->mutex);

    return status;	
}

/*
omeTaskStatus omeThreadPoolGetTaskStatus(omeThreadPool *tp, int taskId)
{
    // TODO: need to check the content of tp->running and tp->waitingTasks, is this even usefull?

    omeTaskStatus status;

    pthread_mutex_lock(&tp->mutex);

    if(taskId < 0 || taskId > tp->lastId)
        status = OME_TASK_STATUS_INVALID_TASK;
    else if()
    else
        status = OME_TASK_STATUS_FINISHED;

    pthread_mutex_unlock(&tp->mutex);

    return status;
}
*/

void *omeThreadPoolMain(void *threadPool)
{
    omeThreadPool *tp = threadPool;
    omeTask *t;
    void *task;
    int i;

    while(tp->terminated == OME_FALSE)
    {
        pthread_mutex_lock(&tp->mutex);

        while(omeQueueIsEmpty(tp->waitingTasks) == OME_TRUE)
        	pthread_cond_wait(&tp->cond, &tp->mutex);

        omeQueuePop(tp->waitingTasks, &task);
        t = task;

        // register to running tasks
        for(i = 0; i < tp->maxThreads; i++)
        	if(tp->running[i] == -1)
        		tp->running[i] = t->id;

        pthread_mutex_unlock(&tp->mutex);
        t->func(tp->context, t->arg);
        pthread_mutex_lock(&tp->mutex);

        // unregister from running tasks
        for(i = 0; i < tp->maxThreads; i++)
        	if(tp->running[i] == t->id)
        		tp->running[i] = -1;

        pthread_mutex_unlock(&tp->mutex);
    }

    return NULL;
}
