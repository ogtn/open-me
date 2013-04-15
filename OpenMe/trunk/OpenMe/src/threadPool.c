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
    tp->terminated = OME_FALSE;
    tp->threads = calloc(maxThreads, sizeof(pthread_t));
    tp->runningTasks = calloc(maxThreads, sizeof(omeTask*));

    // mutex
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutex_init(&tp->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    // condition
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&tp->condEmpty, &cond_attr);
    pthread_cond_init(&tp->condNotEmpty, &cond_attr);
    pthread_condattr_destroy(&cond_attr);

    // barrier
    pthread_barrier_init(&tp->barrier, NULL, maxThreads + 1);

    // threads
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0; i < maxThreads; i++)
        pthread_create(&tp->threads[i], &thread_attr, omeThreadPoolMain, tp);

    pthread_attr_destroy(&thread_attr);
    pthread_barrier_wait(&tp->barrier);

    return tp;
}


void omeThreadPoolDestroy(omeThreadPool **tp)
{
    int i;
    void *retVal;

    (*tp)->terminated = OME_TRUE;
    pthread_cond_broadcast(&(*tp)->condEmpty);

    for(i = 0; i < (*tp)->maxThreads; i++)
        pthread_join((*tp)->threads[i], &retVal);

    pthread_cond_destroy(&(*tp)->condEmpty);
    pthread_cond_destroy(&(*tp)->condNotEmpty);
    pthread_mutex_destroy(&(*tp)->mutex);
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

/*
omeTaskStatus omeThreadPoolGetTaskStatus(omeThreadPool *tp, int taskId)
{
    omeTaskStatus status;
    int i;

    pthread_mutex_lock(&tp->mutex);

    // invalid id
    if(taskId < 0 || taskId > tp->lastId)
    {
        pthread_mutex_unlock(&tp->mutex);
        return OME_TASK_STATUS_INVALID_TASK;
    }

    // task is running
    for(i = 0; i < tp->maxThreads; i++)
    {
        if(tp->runningTasks[i].id == taskId)
        {
            pthread_mutex_unlock(&tp->mutex);
            return OME_TASK_STATUS_RUNNING;
        }
    }

    // task is waiting
    if(???)
    {
        pthread_mutex_unlock(&tp->mutex);
        return OME_TASK_STATUS_WAITING;
    }

    pthread_mutex_unlock(&tp->mutex);
    return OME_TASK_STATUS_COMPLETED;
}
*/

int omeThreadPoolGetRemainingTasks(omeThreadPool *tp)
{
    return omeQueueGetNbElements(tp->waitingTasks);
}


int omeThreadPoolGetRunningTheads(omeThreadPool *tp)
{
    return tp->nbRunningTasks;
}


void omeThreadPoolWaitForCompletion(omeThreadPool *tp)
{
    pthread_mutex_lock(&tp->mutex);
    tp->terminated = OME_TRUE;

    while(tp->nbRunningTasks > 0 || !omeQueueIsEmpty(tp->waitingTasks))
    {
        // omeStringAppendStr(tp->str, "%d tasks remaining, %d running\n", omeQueueGetNbElements(tp->waitingTasks), tp->nbRunningTasks);
        pthread_cond_wait(&tp->condNotEmpty, &tp->mutex);
        // omeStringAppendStr(tp->str, "woken up!:%d tasks remaining, %d running\n", omeQueueGetNbElements(tp->waitingTasks), tp->nbRunningTasks);
    }

    pthread_mutex_unlock(&tp->mutex);
}






#include <stdio.h>

void *omeThreadPoolMain(void *threadPool)
{
    omeThreadPool *tp = threadPool;
    omeTask *t;
    int i;
    pthread_t tid;

    tid = pthread_self();
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    pthread_barrier_wait(&tp->barrier);

    pthread_mutex_lock(&tp->mutex);
    // omeStringAppendStr(tp->str, "[%ld] start\n", tid);

    while(1)
    {
        while(omeQueueIsEmpty(tp->waitingTasks) == OME_TRUE)
        {
            if(tp->terminated == OME_TRUE)
            {
                // omeStringAppendStr(tp->str, "[%ld] stop\n", tid);
                pthread_mutex_unlock(&tp->mutex);
                return NULL;
            }

            // omeStringAppendStr(tp->str, "[%ld] wait for tasks...\n", tid);
        	pthread_cond_wait(&tp->condEmpty, &tp->mutex);
        	// omeStringAppendStr(tp->str, "[%ld] awake!\n", tid);
        }

        omeQueuePop(tp->waitingTasks, (void**)&t);

        // register to running tasks
        for(i = 0; i < tp->maxThreads; i++)
        {
            if(tp->runningTasks[i] == NULL)
            {
                tp->runningTasks[i] = t;
                tp->nbRunningTasks++;
                break;
            }
        }

        if(i == tp->maxThreads)
        {
            fprintf(stderr, "$$$$$$$$$$$$$$ shit happened \n");
            pthread_mutex_unlock(&tp->mutex);
            return NULL;
        }

        // omeStringAppendStr(tp->str, "[%ld] run task %d\n", tid, t->id);
        pthread_mutex_unlock(&tp->mutex);
        t->func(tp->context, t->arg);
        pthread_mutex_lock(&tp->mutex);

        // unregister from running tasks
        for(i = 0; i < tp->maxThreads; i++)
        {
            if(tp->runningTasks[i] == t)
            {
                omeTaskDestroy(&tp->runningTasks[i]);
                tp->nbRunningTasks--;

                // if this is the last task, notify potentially waiting processes
                // if(tp->nbRunningTasks == 0 && omeQueueIsEmpty(tp->waitingTasks))
                    pthread_cond_broadcast(&tp->condNotEmpty);

                break;
            }
        }

        if(i == tp->maxThreads)
        {
            fprintf(stderr, "$$$$$$$$$$$$$$ shit happened \n");
            pthread_mutex_unlock(&tp->mutex);
            return NULL;
        }

        // omeStringAppendStr(tp->str, "[%ld] completed task %d\n", tid, t->id);
    }

    // omeStringAppendStr(tp->str, "[%ld] $$$$$$$$$$$$$WTF$$$$$$$$$$$$$$$$\n", tid);
    return NULL;
}
