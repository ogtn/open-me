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
    int id;
} omeTask;


typedef enum omeThreadPoolStatus
{
    OME_THREAD_POOL_STATUS_RUNNING,     // nominal state
    OME_THREAD_POOL_STATUS_FLUSHING,    // no more tasks can be added while flushing
    OME_THREAD_POOL_STATUS_TERMINATING, // no more tasks will EVER be added
    OME_THREAD_POOL_STATUS_PAUSED       // no tasks are dequeued while paused
} omeThreadPoolStatus;


typedef struct omeThreadPool
{
    omeThreadPoolStatus status;     // thread pool internal state

    // threads
    int nbThreads;                  // number of threads in the pool
    pthread_t *threads;             // threads ids

    // tasks
    omeQueue *waitingTasks;         // waiting queue holding the next tasks to be processed
    int nbRunningTasks;             // number of tasks currently running
    omeTask **runningTasks;         // array of running tasks
    void *context;                  // user context, transmitted to task
    int lastStarted;                // id of the last started task
    int lastId;                     // last delivered id

    // sync stuff
    pthread_cond_t condEmpty;       // use to wait for an empty waiting queue
    pthread_cond_t condNotEmpty;    // use to wait for at least one task in the waiting queue
    pthread_cond_t condNotFull;     // use to wait for an available slot in the waiting queue
    pthread_cond_t condPause;       // use to wait for the pool to be unpaused
    pthread_mutex_t mutex;          // main mutex, used for all critical sections
    pthread_barrier_t barrier;      // used to start threads once the thread pool is fully created
} omeThreadPool;


// Constructor and destructor of a task TODO: private???
omeTask *omeTaskCreate(omeThreadPoolProcessTask func, void *arg, int id);
void omeTaskDestroy(omeTask **t);

// Constructor and destructor of a thread pool
omeThreadPool *omeThreadPoolCreate(int nbThreads, int maxTasks, void *context);
void omeThreadPoolDestroy(omeThreadPool **tp);

// TODO: provide a callback to notify the caller of the task status
// add a task to the pool and returns, it will be processed later
// return a task id
omeStatus omeThreadPoolAddTask(omeThreadPool *tp, omeThreadPoolProcessTask func, void *arg, int *id);

// require the status of a task associated with an id, see omeTaskStatus
omeTaskStatus omeThreadPoolGetTaskStatus(omeThreadPool *tp, int taskId);

// wait for the completion of all waiting tasks then returns
// no tasks can be added while flushing
omeStatus omeThreadPoolFlush(omeThreadPool *tp);

// pause and unpause the processing of new tasks
// while paused, it is still possible to add tasks to the pool
omeStatus omeThreadPoolPause(omeThreadPool *tp);
omeStatus omeThreadPoolUnPause(omeThreadPool *tp);

// return the number of tasks currently waiting
int omeThreadPoolGetRemainingTasks(omeThreadPool *tp);

// return the number of tasks currently running
int omeThreadPoolGetRunningTheads(omeThreadPool *tp);


#ifdef __cplusplus
}
#endif


#endif // OME_THREAD_POOL_H
