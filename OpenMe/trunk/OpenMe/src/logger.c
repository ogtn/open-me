//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "logger.h"
#include <stdarg.h>


static omeLogger logger;


void omeLoggerStart(void)
{
    if(logger.state == OME_LOGGER_STATE_STOPPED)
    {
        logger.log = stderr; // TODO: allow logging in (multiple) file
    }
}


void omeLoggerLog(char *format, ...) // TODO: add criticity levels, filters, current time and macros with __FILE__ and __LINE__ and __FUNCTION__!
{
    va_list list;

    va_start(list, format);
    fprintf(logger.log, "[%3d] ", logger.event++);
    vfprintf(logger.log, format, list);
    va_end(list);
}


void omeLoggerStop(void) // TODO: close properly all the files...
{
    if(logger.state == OME_LOGGER_STATE_STARTED)
    {
        fflush(logger.log);
    }
}
