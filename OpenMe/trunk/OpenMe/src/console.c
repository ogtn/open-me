//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "console.h"
#include "utils.h"
#include "logger.h"
#include <stdio.h>


omeConsole *omeConsoleCreate(void)
{
	omeConsole *c = calloc(1, sizeof(omeConsole));

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

	if(strlen(cmdLine) > OME_CONSOLE_VAR_TYPE_STRING)
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
		omeConsoleVarPrint(v);

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


omeConsoleStatus omeConsoleRegisterVar(omeConsole *c, const char *name, omeConsoleVarType type, void *data)
{
	omeConsoleVar *v;
	size_t size = strlen(name);

	if(size == 0 || size > OME_CONSOLE_MAX_VAR_NAME)
		return OME_CONSOLE_STATUS_INVALID_NAME;

	HASH_FIND_STR(c->variables, name, v);

	if(v != NULL)
		return OME_CONSOLE_STATUS_DUPLICATED_VARIABLE;

	v = omeConsoleVarCreate(name, type, data);
	HASH_ADD_STR(c->variables, key, v);

	return OME_CONSOLE_STATUS_NO_ERROR;
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


omeConsoleStatus omeConsoleVarPrint(const omeConsoleVar *v)
{
	switch(v->type)
	{
		case OME_CONSOLE_VAR_TYPE_INT:
			printf("int %s = %d\n", v->key, *(int *)v->value);
			break;
		case OME_CONSOLE_VAR_TYPE_FLOAT:
			printf("float %s = %f\n", v->key, *(float *)v->value);
			break;
		case OME_CONSOLE_VAR_TYPE_STRING:
			printf("string %s = \"%s\"\n", v->key, (char *)v->value);
			break;
		default:
			return OME_CONSOLE_STATUS_BAD_TYPE;
	};

	return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleCmd *omeConsoleCmdCreate(omeConsoleCallback cb, void *userData)
{
	omeConsoleCmd *cmd = calloc(1, sizeof(omeConsoleCmd));

	cmd->cb = cb;
	cmd->userData = userData;

	return cmd;
}


void omeConsoleCmdDestroy(omeConsoleCmd **cmd)
{
	omeDbgClearMem(*cmd, sizeof(omeConsoleCmd));
	free(*cmd);
	*cmd = NULL;
}
