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
#include "logger.h"


// Constructor and destructor of a task
static omeTask *omeTaskCreate(omeThreadPoolProcessTask func, void *arg, int id);
static void omeTaskDestroy(omeTask **t);

// main of each thread, try to dequeue and process tasks when possible
// (threads pool unpaused, tasks in waiting queue)
// quits when the threads pool is destroyed and the queue empty
static void *omeThreadPoolMain(void *threadPool);

// add and remove a task from the running array
static void omeThreadPoolAddToRunning(omeThreadPool *tp, omeTask *t);
static void omeThreadPoolRemoveFromRunning(omeThreadPool *tp, omeTask *t);


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


omeThreadPool *omeThreadPoolCreate(int nbThreads, int maxTasks, void *context)
{
    int i;
    omeThreadPool *tp;
    pthread_attr_t thread_attr;

    // thread pool and tasks
    tp = calloc(1, sizeof(omeThreadPool));
    tp->nbThreads = nbThreads;
    tp->context = context;
    tp->lastId = -1;
    tp->lastStarted = -1;
    tp->waitingTasks = omeQueueCreate(maxTasks);
    tp->status = OME_THREAD_POOL_STATUS_RUNNING;
    tp->threads = calloc(nbThreads, sizeof(pthread_t));
    tp->runningTasks = calloc(nbThreads, sizeof(omeTask*));

    // sync
    pthread_mutex_init(&tp->mutex, NULL);
    pthread_cond_init(&tp->condEmpty, NULL);
    pthread_cond_init(&tp->condNotEmpty, NULL);
    pthread_cond_init(&tp->condNotFull, NULL);
    pthread_cond_init(&tp->condPause, NULL);
    pthread_barrier_init(&tp->barrier, NULL, nbThreads + 1);

    // threads
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < nbThreads; i++)
        pthread_create(&tp->threads[i], &thread_attr, omeThreadPoolMain, tp);

    pthread_attr_destroy(&thread_attr);
    pthread_barrier_wait(&tp->barrier);

    return tp;
}

void omeThreadPoolDestroy(omeThreadPool **tp)
{
    int i;
    void *retVal;

    pthread_mutex_lock(&(*tp)->mutex);
    (*tp)->status = OME_THREAD_POOL_STATUS_TERMINATING;
    pthread_mutex_unlock(&(*tp)->mutex);

    // wake up all possibly waiting threads
    pthread_cond_broadcast(&(*tp)->condPause);
    pthread_cond_broadcast(&(*tp)->condEmpty);

    for(i = 0; i < (*tp)->nbThreads; i++)
        pthread_join((*tp)->threads[i], &retVal);

    pthread_mutex_destroy(&(*tp)->mutex);

    pthread_cond_destroy(&(*tp)->condEmpty);
    pthread_cond_destroy(&(*tp)->condNotEmpty);
    pthread_cond_destroy(&(*tp)->condNotFull);
    pthread_cond_destroy(&(*tp)->condPause);

    omeQueueDestroy(&(*tp)->waitingTasks);

    free((*tp)->runningTasks);
    free(*tp);
    *tp = NULL;
}


omeStatus omeThreadPoolAddTask(omeThreadPool *tp, omeThreadPoolProcessTask func, void *arg, int *taskId)
{
    omeTask *t;
    omeStatus status;

    *taskId = -1;

    if(func == NULL)
        return OME_FAILURE;

    pthread_mutex_lock(&tp->mutex);

    t = omeTaskCreate(func, arg, tp->lastId + 1);
    status = omeQueuePush(tp->waitingTasks, t);

    if(status == OME_SUCCESS)
    {
        tp->lastId++;
        *taskId = tp->lastId;
        pthread_cond_signal(&tp->condEmpty);
    }
    else
        omeTaskDestroy(&t);

    pthread_mutex_unlock(&tp->mutex);

    return status;  
}


omeTaskStatus omeThreadPoolGetTaskStatus(omeThreadPool *tp, int taskId)
{
    omeTaskStatus status;
    omeTask *t;
    int i;

    pthread_mutex_lock(&tp->mutex);

    // invalid id
    if(taskId < 0 || taskId > tp->lastId)
    {
        pthread_mutex_unlock(&tp->mutex);
        return OME_TASK_STATUS_INVALID_TASK;
    }

    // task is running
    for(i = 0; i < tp->nbThreads; i++)
    {
        if(tp->runningTasks[i] && tp->runningTasks[i]->id == taskId)
        {
            pthread_mutex_unlock(&tp->mutex);
            return OME_TASK_STATUS_RUNNING;
        }
    }

    // task is waiting
    omeQueueForEach(tp->waitingTasks, i, t)
    {
        if(t->id == taskId)
        {
            pthread_mutex_unlock(&tp->mutex);
            return OME_TASK_STATUS_WAITING;
        }
    }

    pthread_mutex_unlock(&tp->mutex);
    return OME_TASK_STATUS_COMPLETED;
}


int omeThreadPoolGetRemainingTasks(omeThreadPool *tp)
{
    return omeQueueGetNbElements(tp->waitingTasks);
}


int omeThreadPoolGetRunningTheads(omeThreadPool *tp)
{
    return tp->nbRunningTasks;
}


omeStatus omeThreadPoolFlush(omeThreadPool *tp)
{
    pthread_mutex_lock(&tp->mutex);

    if(tp->status != OME_THREAD_POOL_STATUS_RUNNING)
    {
        pthread_mutex_unlock(&tp->mutex);
        return OME_FAILURE;
    }

    tp->status = OME_THREAD_POOL_STATUS_FLUSHING;

    while(tp->nbRunningTasks > 0 || !omeQueueIsEmpty(tp->waitingTasks))
    {
        pthread_cond_wait(&tp->condNotEmpty, &tp->mutex);

        if(tp->status != OME_THREAD_POOL_STATUS_FLUSHING)
        {
            pthread_mutex_unlock(&tp->mutex);
            return OME_FAILURE;            
        }
    }

    tp->status = OME_THREAD_POOL_STATUS_RUNNING;
    pthread_mutex_unlock(&tp->mutex);

    return OME_SUCCESS;
}


omeStatus omeThreadPoolPause(omeThreadPool *tp)
{
    omeStatus res = OME_FAILURE;

    pthread_mutex_lock(&tp->mutex);

    if(tp->status != OME_THREAD_POOL_STATUS_TERMINATING)
    {
        res = OME_SUCCESS;
        tp->status = OME_THREAD_POOL_STATUS_PAUSED;
    }

    pthread_mutex_unlock(&tp->mutex);

    return res;
}


omeStatus omeThreadPoolUnPause(omeThreadPool *tp)
{
    omeStatus res = OME_FAILURE;

    pthread_mutex_lock(&tp->mutex);

    if(tp->status == OME_THREAD_POOL_STATUS_PAUSED)
    {
        res = OME_SUCCESS;
        tp->status = OME_THREAD_POOL_STATUS_RUNNING;
        pthread_cond_broadcast(&tp->condPause);
    }

    pthread_mutex_unlock(&tp->mutex);

    return res;
}


void *omeThreadPoolMain(void *threadPool)
{
    omeThreadPool *tp = threadPool;
    omeTask *t;

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_barrier_wait(&tp->barrier);
    pthread_mutex_lock(&tp->mutex);

    while(1)
    {
        while(omeQueueIsEmpty(tp->waitingTasks) || tp->status == OME_THREAD_POOL_STATUS_PAUSED)
        {
            switch(tp->status)
            {
                case OME_THREAD_POOL_STATUS_TERMINATING:
                    pthread_mutex_unlock(&tp->mutex);
                    return NULL;
                case OME_THREAD_POOL_STATUS_PAUSED:
                    pthread_cond_wait(&tp->condPause, &tp->mutex);
                    break;
                case OME_THREAD_POOL_STATUS_RUNNING:
                case OME_THREAD_POOL_STATUS_FLUSHING:
                	pthread_cond_wait(&tp->condEmpty, &tp->mutex);
            }
        }

        omeQueuePop(tp->waitingTasks, (void**)&t);
        omeThreadPoolAddToRunning(tp, t);
        pthread_mutex_unlock(&tp->mutex);
        t->func(tp->context, t->arg);
        pthread_mutex_lock(&tp->mutex);
        omeThreadPoolRemoveFromRunning(tp, t);
    }

    omeLoggerLog("Something went wrong in omeThreadPoolMain()\n");

    return NULL;
}


static void omeThreadPoolAddToRunning(omeThreadPool *tp, omeTask *t)
{
    int i;

    for(i = 0; i < tp->nbThreads; i++)
    {
        if(tp->runningTasks[i] == NULL)
        {
            tp->runningTasks[i] = t;
            tp->nbRunningTasks++;
            return;
        }
    }

    omeLoggerLog("Something went wrong in omeThreadPoolAddToRunning()\n");
}


static void omeThreadPoolRemoveFromRunning(omeThreadPool *tp, omeTask *t)
{
    int i;

    for(i = 0; i < tp->nbThreads; i++)
    {
        if(tp->runningTasks[i] == t)
        {
            omeTaskDestroy(&tp->runningTasks[i]);
            tp->nbRunningTasks--;

            // if this was the last running task, notify potentially waiting processes
            if(tp->nbRunningTasks == 0 && omeQueueIsEmpty(tp->waitingTasks))
                pthread_cond_broadcast(&tp->condNotEmpty);

            return;
        }
    }

    omeLoggerLog("Something went wrong in omeThreadPoolRemoveFromRunning()\n");
}
