//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_LOGGER_H
#define OME_LOGGER_H


#ifdef __cplusplus
extern "C" {
#endif


#include "utils.h"
#include <stdio.h>


typedef enum omeLoggerState
{
    OME_LOGGER_STATE_STOPPED,
    OME_LOGGER_STATE_STARTED,
    OME_LOGGER_STATE_MAX
} omeLoggerState;


// TODO: use this combined to log levels
// see http://stackoverflow.com/questions/3585846/color-text-in-terminal-aplications-in-unix
#define OME_LOGGER_NRM  "\x1B[0m"
#define OME_LOGGER_RED  "\x1B[31m"
#define OME_LOGGER_GRN  "\x1B[32m"
#define OME_LOGGER_YEL  "\x1B[33m"
#define OME_LOGGER_BLU  "\x1B[34m"
#define OME_LOGGER_MAG  "\x1B[35m"
#define OME_LOGGER_CYN  "\x1B[36m"
#define OME_LOGGER_WHT  "\x1B[37m"


typedef struct omeLogger
{
    FILE *log;
    int event;
    omeLoggerState state;
} omeLogger;



void omeLoggerStart(void);
/*
void omeLog(int level, const char *format, ...);
#define omeLoggerLog(int level, int line, const char *file, const char* func, const char *format, ...); // fuckin' variadics...
*/

void omeLoggerLog(const char *format, ...) OME_CHK_FMT_AND_ARGS(1);

void omeLoggerStop(void);


#ifdef __cplusplus
}
#endif


#endif // OME_LOGGER_H
