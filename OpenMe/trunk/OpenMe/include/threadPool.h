//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_THREAD_POOL_H
#define OME_THREAD_POOL_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include "queue.h"
#include <pthread.h>


typedef void (*omeThreadPoolProcessTask) (void *context, void *arg);


typedef enum omeTaskStatus
{
    OME_TASK_STATUS_INVALID_TASK,
    OME_TASK_STATUS_WAITING,
    OME_TASK_STATUS_RUNNING,
    OME_TASK_STATUS_FINISHED
} omeTaskStatus;


typedef struct omeTask
{
    omeThreadPoolProcessTask func;
    void *arg;
    int *taskId;
} omeTask;


typedef struct omeThreadPool
{
    int maxThreads;
    pthread_t *threads;
    int maxTasks;
    omeQueue *waitingTasks;
    void **tasksData;
    void *context;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    int lastStarted;
    int lastId;
    int *running;
} omeThreadPool;


omeTask *omeTaskCreate(omeThreadPoolProcessTask func, void *arg, int *taskId);
void omeTaskDestroy(omeTask **t);

omeThreadPool *omeThreadPoolCreate(int maxThreads, int maxTasks, void *context);
void omeThreadPoolDestroy(omeThreadPool **tp);
// TODO: provide a callback to notify the caller of the task status
omeStatus omeThreadPoolAddTask(omeThreadPool *tp, omeThreadPoolProcessTask func, void *arg, int *taskId);
omeTaskStatus omeThreadPoolGetTaskStatus(omeThreadPool *tp, int taskId);
void *omeThreadPoolMain(void *threadPool);


#ifdef __cplusplus
}
#endif


#endif // OME_THREAD_POOL_H
