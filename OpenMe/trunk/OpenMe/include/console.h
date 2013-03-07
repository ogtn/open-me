//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_CONSOLE_H
#define OME_CONSOLE_H


#ifdef __cplusplus
extern "C" {
#endif


#include "uthash.h"


#define OME_CONSOLE_MAX_VAR_NAME		32
#define OME_CONSOLE_MAX_LINE_SIZE		256
#define OME_CONSOLE_MAX_ARGS			8


typedef enum omeConsoleStatus
{
	OME_CONSOLE_STATUS_NO_ERROR,
	OME_CONSOLE_STATUS_BAD_TYPE,
	OME_CONSOLE_STATUS_ENV_FULL,
	OME_CONSOLE_STATUS_NOT_FOUND,
	OME_CONSOLE_STATUS_INVALID_CB,
	OME_CONSOLE_STATUS_TOO_MUCH_ARGS,
	OME_CONSOLE_STATUS_DUPLICATED_COMMAND,
	OME_CONSOLE_STATUS_DUPLICATED_VARIABLE,
	OME_CONSOLE_STATUS_INVALID_CMD,
	OME_CONSOLE_STATUS_LINE_OVERFLOW,
	OME_CONSOLE_STATUS_CANT_CONVERT,
	OME_CONSOLE_STATUS_INVALID_NAME
} omeConsoleStatus;


typedef enum omeConsoleVarType
{
	OME_CONSOLE_VAR_TYPE_INT,
	OME_CONSOLE_VAR_TYPE_FLOAT,
	OME_CONSOLE_VAR_TYPE_STRING
} omeConsoleVarType;


typedef struct omeConsoleVar
{
	UT_hash_handle hh;
	char key[OME_CONSOLE_MAX_VAR_NAME];
	void *value;
	omeConsoleVarType type;
} omeConsoleVar, omeConsoleVarHashTable;


typedef struct omeConsoleCmdLine
{
	int argsNb;
	char str[OME_CONSOLE_MAX_LINE_SIZE];
	char cmd[OME_CONSOLE_MAX_VAR_NAME];
	char args[OME_CONSOLE_MAX_ARGS][OME_CONSOLE_MAX_VAR_NAME];
} omeConsoleCmdLine;


typedef struct omeConsole omeConsole;
typedef void(*omeConsoleCallback)(omeConsole *c, void *userData);


typedef struct omeConsoleCmd
{
	UT_hash_handle hh;
	char key[OME_CONSOLE_MAX_VAR_NAME];
	omeConsoleCallback cb;
	void *userData;
} omeConsoleCmd, omeConsoleCmdHashTable;


struct omeConsole
{
	omeConsoleVarHashTable *variables;
	omeConsoleCmdHashTable *commands;
	omeConsoleCmdLine cmdLine; // TODO: use a circular buffer to store a commands history
};


// public interface
omeConsole *omeConsoleCreate(void);
void omeConsoleDestroy(omeConsole **c);
omeConsoleStatus omeConsoleProcess(omeConsole *c, const char *cmdLine);
omeConsoleStatus omeConsoleRegisterInt(omeConsole *c, const char *name, int *value);
omeConsoleStatus omeConsoleRegisterFloat(omeConsole *c, const char *name, float *value);
omeConsoleStatus omeConsoleRegisterString(omeConsole *c, const char *name, char *value);
// omeConsoleStatus omeConsoleRegisterOnVarCallback(omeConsole *c, omeConsoleCallback cb, void *userData);
// omeConsoleStatus omeConsoleRegisterPrintCallback(omeConsole *c, omeConsoleCallback cb, void *userData);

// private functions
omeConsoleStatus omeConsoleRegisterVar(omeConsole *c, const char *name, omeConsoleVarType type, void *data);
omeConsoleStatus omeConsoleRegisterCallback(omeConsole *c, const char *name, omeConsoleCallback cb, void *userData);
omeConsoleStatus omeConsoleUnregisterCallback(omeConsole *c, const char *name);
omeConsoleStatus omeConsoleParse(omeConsoleCmdLine *cmdLine);
omeConsoleStatus omeConsoleInterpret(omeConsole *c);

omeConsoleVar *omeConsoleVarCreate(const char *name, omeConsoleVarType type, void *value);
void omeConsoleVarDestroy(omeConsoleVar **v);
omeConsoleStatus omeConsoleVarPrint(const omeConsoleVar *v);

omeConsoleCmd *omeConsoleCmdCreate(omeConsoleCallback cb, void *userData);
void omeConsoleCmdDestroy(omeConsoleCmd **cmd);


#ifdef __cplusplus
}
#endif


#endif // OME_CONSOLE_H
