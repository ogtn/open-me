//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
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
#include "utils.h"


// Fixed limits of the console
#define OME_CONSOLE_MAX_VAR_NAME	32
#define OME_CONSOLE_MAX_LINE_SIZE	256
#define OME_CONSOLE_MAX_ARGS		8


// All status returned by the omeConsoleXXX() functions
// Names should be sufficient to understand the meaning
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
	OME_CONSOLE_STATUS_INVALID_NAME,

	// keep in last position
	OME_MAX_CONSOLE_STATUS
} omeConsoleStatus;


// Types of variables accepted by the console environment
typedef enum omeConsoleVarType
{
	OME_CONSOLE_VAR_TYPE_INT,
	OME_CONSOLE_VAR_TYPE_BOOL,
	OME_CONSOLE_VAR_TYPE_FLOAT,
	OME_CONSOLE_VAR_TYPE_STRING
	// TODO: add OME_CONSOLE_VAR_TYPE_USER ???
} omeConsoleVarType;


// value held by a variable
typedef union omeConsoleVarValue
{
	int intVal;
	int boolVal;
	float floatVal;
	char strVal[OME_CONSOLE_MAX_LINE_SIZE];
} omeConsoleVarValue;


// Variables, meant to be stored in a hashmap in the environment
typedef struct omeConsoleVar
{
	UT_hash_handle hh;					// needed by uthash
	char key[OME_CONSOLE_MAX_VAR_NAME];	// key used by the hashmap
	omeConsoleVarType type;				// type of the variable
	omeConsoleVarValue value;			// value held
} omeConsoleVar, omeConsoleVarHashTable;


// Command line as sent by the user and as seen by the parser
typedef struct omeConsoleCmdLine
{
	char str[OME_CONSOLE_MAX_LINE_SIZE];	// command line as typed by the user
	char cmd[OME_CONSOLE_MAX_VAR_NAME];		// command extracted by the parser
	int argsNb;								// number of arguments found by the parser
	char args[OME_CONSOLE_MAX_ARGS][OME_CONSOLE_MAX_VAR_NAME];	// list of args found by the parser
} omeConsoleCmdLine;


// Callback associated with a console command
typedef struct omeConsole omeConsole;
typedef void(*omeConsoleCallback)(omeConsole *c, void *userData);


// Command, meant to be stored in a hashmap in the environment
typedef struct omeConsoleCmd
{
	UT_hash_handle hh;					// needed by uthash
	char key[OME_CONSOLE_MAX_VAR_NAME];	// key used by the hashmap
	omeConsoleCallback cb;				// callback called upon command use
	void *userData;						// generic optional data to pass to the callback
} omeConsoleCmd, omeConsoleCmdHashTable;


typedef int(*omeConsolePrintFunc)(const char *, ...) OME_CHK_FMT_AND_ARGS(1);

// Hold the context of the console
struct omeConsole
{
	omeConsoleVarHashTable *variables;	// hashmap of registered variables
	omeConsoleCmdHashTable *commands;	// hashmap of registred commands
	omeConsoleCmdLine cmdLine; 			// current command line
	omeConsolePrintFunc printCb;		// printf-like to use for print tasks
	// TODO: use a circular buffer to store a command history
};


//////////////////////// [Public interface] ////////////////////////

// Constructor and destructor of the console environment
omeConsole *omeConsoleCreate(void);
void omeConsoleDestroy(omeConsole **c);

// Execute the command line given in argument
omeConsoleStatus omeConsoleProcess(omeConsole *c, const char *cmdLine);

// This set of functions allow the caller to add variables to the console environement
omeConsoleStatus omeConsoleRegisterInt(omeConsole *c, const char *name, int value);
omeConsoleStatus omeConsoleRegisterFloat(omeConsole *c, const char *name, float value);
omeConsoleStatus omeConsoleRegisterString(omeConsole *c, const char *name, const char *value);

// This set of functions allow the caller to get the value associated with the given variable name
omeConsoleStatus omeConsoleGetInt(omeConsole *c, const char *name, int *value);
omeConsoleStatus omeConsoleGetFloat(omeConsole *c, const char *name, float *value);
omeConsoleStatus omeConsoleGetString(omeConsole *c, const char *name, const char **value);

// This set of functions allow the caller to set the value associated with the given variable name
omeConsoleStatus omeConsoleSetInt(omeConsole *c, const char *name, int value);
omeConsoleStatus omeConsoleSetFloat(omeConsole *c, const char *name, float value);
omeConsoleStatus omeConsoleSetString(omeConsole *c, const char *name, const char *value);

// Print the environment
omeConsoleStatus omeConsolePrint(const omeConsole *c);

// Print a variable
omeConsoleStatus omeConsoleVarPrint(const omeConsole *c, const omeConsoleVar *v);

// Register a callback to be called upon a given command
omeConsoleStatus omeConsoleRegisterCallback(omeConsole *c, const char *name, omeConsoleCallback cb, void *userData);

// TODO: Register a callback called when a variable name is entered as a command
// omeConsoleStatus omeConsoleRegisterOnVarCallback(omeConsole *c, omeConsoleCallback cb, void *userData);

// Register the printf-like function used by omeConsoleVarPrint()
omeConsoleStatus omeConsoleRegisterPrintCallback(omeConsole *c, omeConsolePrintFunc cb);

// TODO: Remove a callback from the environment?
// omeConsoleStatus omeConsoleUnregisterCallback(omeConsole *c, const char *name);

// Return the string associated with the given error code
const char *omeConsoleErr2Str(omeConsoleStatus status);

//////////////////////// [Private functions] ////////////////////////

// Generic function to add variable to the environment
omeConsoleStatus omeConsoleRegisterVar(omeConsole *c, const char *name, omeConsoleVarType type, const void *value);
omeConsoleVar *omeConsoleGetVar(omeConsole *c, const char *name);
omeConsoleStatus omeConsoleSetVar(omeConsole *c, const char *name, omeConsoleVarType type, const void *value);

// Parse a command line into command and arguments
omeConsoleStatus omeConsoleParse(omeConsoleCmdLine *cmdLine);

// Execute a command line, by calling the associated callback or setting/printing the given var
omeConsoleStatus omeConsoleInterpret(omeConsole *c);

// Constructor and destructor of a console variable
omeConsoleVar *omeConsoleVarCreate(const char *name, omeConsoleVarType type, const void *value);
void omeConsoleVarDestroy(omeConsoleVar **v);

// Constructor and destructor of a console command
omeConsoleCmd *omeConsoleCmdCreate(const char *name, omeConsoleCallback cb, void *userData);
void omeConsoleCmdDestroy(omeConsoleCmd **cmd);


#ifdef __cplusplus
}
#endif


#endif // OME_CONSOLE_H
