//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "console.h"
#include "logger.h"
#include <stdio.h>


omeConsole *omeConsoleCreate(void)
{
	omeConsole *c = calloc(1, sizeof(omeConsole));
	c->printCb = printf;

	return c;
}


void omeConsoleDestroy(omeConsole **c)
{
    omeConsoleVar *v, *v_temp;
    omeConsoleCmd *cmd, *cmd_temp;

    HASH_ITER(hh, (*c)->variables, v, v_temp)
    {
    	HASH_DEL((*c)->variables, v);
    	omeConsoleVarDestroy(&v);
    }

    HASH_ITER(hh, (*c)->commands, cmd, cmd_temp)
    {
    	HASH_DEL((*c)->commands, cmd);
    	omeConsoleCmdDestroy(&cmd);
    }

    omeDbgClearMem(*c, sizeof(omeConsole));
    free(*c);
    *c = NULL;
}


omeConsoleStatus omeConsoleProcess(omeConsole *c, const char *cmdLine)
{
	omeConsoleStatus status;

	if(strlen(cmdLine) > OME_CONSOLE_MAX_LINE_SIZE)
		return OME_CONSOLE_STATUS_LINE_OVERFLOW;

	strcpy(c->cmdLine.str, cmdLine);
	omeCleanString(c->cmdLine.str);
	status = omeConsoleParse(&c->cmdLine);

	if(status != OME_CONSOLE_STATUS_NO_ERROR)
		return status;

	return omeConsoleInterpret(c);
}


omeConsoleStatus omeConsoleParse(omeConsoleCmdLine *cmdLine)
{
	char *str;

	cmdLine->argsNb = 0;
	str = cmdLine->str;

	omeSkipBlanks(&str);

	if(sscanf(str, "%s", cmdLine->cmd) == 1)
		str += strlen(cmdLine->cmd);

	while(*str)
	{
		if(omeIsBlank(*str))
		{
			omeSkipBlanks(&str);
			continue;
		}

		if(*str == '"')
		{
			int i = 0;
			str++;

			for(i = 0; *str && *str != '"' && i < OME_CONSOLE_MAX_VAR_NAME; i++, str++)
				cmdLine->args[cmdLine->argsNb][i] = *str;

			cmdLine->argsNb++;

			if(*str == '"')
				str++;
			
			continue;
		}

		if(sscanf(str, "%s", cmdLine->args[cmdLine->argsNb]) == 1)
		{
			str += strlen(cmdLine->args[cmdLine->argsNb]);
			cmdLine->argsNb++;

			if(cmdLine->argsNb >= OME_CONSOLE_MAX_ARGS)
				return OME_CONSOLE_STATUS_TOO_MUCH_ARGS;
		}
		else
		{
			omeLoggerLog("Shit happened in omeConsoleParse()\n");
			str++;
		}
	}

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleInterpret(omeConsole *c)
{
	omeConsoleVar *v;
	omeConsoleCmd *cmd;
	omeConsoleCmdLine *cmdLine = &c->cmdLine;

	HASH_FIND_STR(c->commands, cmdLine->cmd, cmd);

	if(cmd != NULL)
	{
		cmd->cb(c, cmd->userData);		

		return OME_CONSOLE_STATUS_NO_ERROR;
	}

	HASH_FIND_STR(c->variables, cmdLine->cmd, v);

	if(v != NULL)
	{
		omeConsoleVarPrint(c, v);

		return OME_CONSOLE_STATUS_NO_ERROR;
	}

	return OME_CONSOLE_STATUS_INVALID_CMD;
}


omeConsoleStatus omeConsoleRegisterInt(omeConsole *c, const char *name, int *value)
{
	return omeConsoleRegisterVar(c, name, OME_CONSOLE_VAR_TYPE_INT, value);
}


omeConsoleStatus omeConsoleRegisterFloat(omeConsole *c, const char *name, float *value)
{
	return omeConsoleRegisterVar(c, name, OME_CONSOLE_VAR_TYPE_FLOAT, value);
}


omeConsoleStatus omeConsoleRegisterString(omeConsole *c, const char *name, char *value)
{
	return omeConsoleRegisterVar(c, name, OME_CONSOLE_VAR_TYPE_STRING, value);
}


omeConsoleStatus omeConsoleRegisterVar(omeConsole *c, const char *name, omeConsoleVarType type, void *value)
{
	omeConsoleVar *v;
	size_t size = strlen(name);

	if(size == 0 || size > OME_CONSOLE_MAX_VAR_NAME)
		return OME_CONSOLE_STATUS_INVALID_NAME;

	HASH_FIND_STR(c->variables, name, v);

	if(v != NULL)
		return OME_CONSOLE_STATUS_DUPLICATED_VARIABLE;

	v = omeConsoleVarCreate(name, type, value);
	HASH_ADD_STR(c->variables, key, v);

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleGetInt(omeConsole *c, const char *name, int *value)
{
	return omeConsoleGetVar(c, name, OME_CONSOLE_VAR_TYPE_INT, value);
}


omeConsoleStatus omeConsoleGetFloat(omeConsole *c, const char *name, float *value)
{
	return omeConsoleGetVar(c, name, OME_CONSOLE_VAR_TYPE_FLOAT, value);
}


omeConsoleStatus omeConsoleGetString(omeConsole *c, const char *name, char *value)
{
	return omeConsoleGetVar(c, name, OME_CONSOLE_VAR_TYPE_STRING, value);
}


omeConsoleStatus omeConsoleGetVar(omeConsole *c, const char *name, omeConsoleVarType type, void *value)
{
	omeConsoleVar *v;

	HASH_FIND_STR(c->variables, name, v);

	if(v == NULL)
		return OME_CONSOLE_STATUS_NOT_FOUND;

	if(v->type != type)
		return OME_CONSOLE_STATUS_BAD_TYPE;

	if(type == OME_CONSOLE_VAR_TYPE_STRING)
		strncpy(value, v->value, OME_CONSOLE_MAX_LINE_SIZE);
	else
		memcpy(value, v->value, sizeof(int));	// TODO: works for ints and floats, but pretty crappy, get rid of this

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleSetInt(omeConsole *c, const char *name, int value)
{
	return omeConsoleSetVar(c, name, OME_CONSOLE_VAR_TYPE_INT, &value);
}


omeConsoleStatus omeConsoleSetFloat(omeConsole *c, const char *name, float value)
{
	return omeConsoleSetVar(c, name, OME_CONSOLE_VAR_TYPE_FLOAT, &value);
}


omeConsoleStatus omeConsoleSetString(omeConsole *c, const char *name, char *value)
{
	return omeConsoleSetVar(c, name, OME_CONSOLE_VAR_TYPE_STRING, value);
}


omeConsoleStatus omeConsoleSetVar(omeConsole *c, const char *name, omeConsoleVarType type, void *value)
{
	omeConsoleVar *v;

	HASH_FIND_STR(c->variables, name, v);

	if(v != NULL)
		return OME_CONSOLE_STATUS_NOT_FOUND;

	if(v->type != type)
		return OME_CONSOLE_STATUS_BAD_TYPE;

	if(type == OME_CONSOLE_VAR_TYPE_STRING)
		strncpy(v->value, value, OME_CONSOLE_MAX_LINE_SIZE);
	else
		memcpy(v->value, value, sizeof(int));	// TODO: works for ints and floats, but pretty crappy, get rid of this

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleRegisterCallback(omeConsole *c, const char *name, omeConsoleCallback cb, void *userData)
{
	omeConsoleCmd *cmd;
	size_t size = strlen(name);

	if(size == 0 || size > OME_CONSOLE_MAX_VAR_NAME)
		return OME_CONSOLE_STATUS_INVALID_NAME;

	if(cb == NULL)
		return OME_CONSOLE_STATUS_INVALID_CB;

	HASH_FIND_STR(c->commands, name, cmd);

	if(cmd != NULL)
		return OME_CONSOLE_STATUS_DUPLICATED_COMMAND;

	cmd = omeConsoleCmdCreate(name, cb, userData);
	HASH_ADD_STR(c->commands, key, cmd);

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleRegisterPrintCallback(omeConsole *c, omeConsolePrintFunc cb)
{
	if(cb == NULL)
		return OME_CONSOLE_STATUS_INVALID_CB;

	c->printCb = cb;

	return OME_CONSOLE_STATUS_NO_ERROR;
}


const char *omeConsoleErr2Str(omeConsoleStatus status)
{
	
	static const char *errMsgs[OME_MAX_CONSOLE_STATUS + 1] = {
		"No error",
		"Bad type",
		"Environment full",
		"Identifier not found",
		"Invalid callback",
		"Too much argugments",
		"Command already exists",
		"Variable already exists",
		"Invalid command",
		"Line too long",
		"Unable to convert the value",
		"Invalid identifier",

		"Unknown error"
	};

	if(status >= OME_MAX_CONSOLE_STATUS)
		status = OME_MAX_CONSOLE_STATUS;

	return errMsgs[status];
}


omeConsoleVar *omeConsoleVarCreate(const char *name, omeConsoleVarType type, void *value)
{
	omeConsoleVar *v = calloc(1, sizeof(omeConsoleVar));

	strcpy(v->key, name);
	v->type = type;
	v->value = value;

	return v;
}


void omeConsoleVarDestroy(omeConsoleVar **v)
{
	omeDbgClearMem(*v, sizeof(omeConsoleVar));
	free(*v);
	*v = NULL;
}


omeConsoleStatus omeConsoleVarPrint(const omeConsole *c, const omeConsoleVar *v)
{
	switch(v->type)
	{
		case OME_CONSOLE_VAR_TYPE_INT:
			c->printCb("int %s = %d\n", v->key, *(int *)v->value);
			break;
		case OME_CONSOLE_VAR_TYPE_FLOAT:
			c->printCb("float %s = %f\n", v->key, *(float *)v->value);
			break;
		case OME_CONSOLE_VAR_TYPE_STRING:
			c->printCb("string %s = \"%s\"\n", v->key, (char *)v->value);
			break;
		default:
			return OME_CONSOLE_STATUS_BAD_TYPE;
	};

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsolePrint(const omeConsole *c)
{
	omeConsoleVar *v;
	omeConsoleCmd *cmd;

	c->printCb("%d variables :\n", HASH_COUNT(c->variables));

	for(v = c->variables; v != NULL; v = v->hh.next)
		omeConsoleVarPrint(c, v);

	c->printCb("%d commands :\n", HASH_COUNT(c->commands));

	for(cmd = c->commands; cmd != NULL; cmd = cmd->hh.next)
		c->printCb("%s()\n", cmd->key);

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleCmd *omeConsoleCmdCreate(const char *name, omeConsoleCallback cb, void *userData)
{
	omeConsoleCmd *cmd = calloc(1, sizeof(omeConsoleCmd));

	cmd->cb = cb;
	cmd->userData = userData;
	strncpy(cmd->key, name, OME_CONSOLE_MAX_VAR_NAME);

	return cmd;
}


void omeConsoleCmdDestroy(omeConsoleCmd **cmd)
{
	omeDbgClearMem(*cmd, sizeof(omeConsoleCmd));
	free(*cmd);
	*cmd = NULL;
}
