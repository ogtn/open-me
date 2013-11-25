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
    omeConsoleVar *var, *var_temp;
    omeConsoleCmd *cmd, *cmd_temp;

    HASH_ITER(hh, (*c)->variables, var, var_temp)
    {
        HASH_DEL((*c)->variables, var);
        omeConsoleVarDestroy(&var);
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

    strncpy(c->cmdLine.str, cmdLine, OME_CONSOLE_MAX_LINE_SIZE);
    c->cmdLine.str[OME_CONSOLE_MAX_LINE_SIZE - 1] = '\0';
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
        // assignment
        if(cmdLine->argsNb == 2 && strcmp(cmdLine->args[0], "=") == 0)
            return omeConsoleSetString(c, cmdLine->cmd, cmdLine->args[1]);
        
        // print
        omeConsoleVarPrint(c, v);

        return OME_CONSOLE_STATUS_NO_ERROR;
    }

    return OME_CONSOLE_STATUS_INVALID_CMD;
}


omeConsoleStatus omeConsoleRegisterInt(omeConsole *c, const char *name, int value)
{
    return omeConsoleRegisterVar(c, name, OME_CONSOLE_VAR_TYPE_INT, &value);
}


omeConsoleStatus omeConsoleRegisterFloat(omeConsole *c, const char *name, float value)
{
    return omeConsoleRegisterVar(c, name, OME_CONSOLE_VAR_TYPE_FLOAT, &value);
}


omeConsoleStatus omeConsoleRegisterString(omeConsole *c, const char *name, const char *value)
{
    return omeConsoleRegisterVar(c, name, OME_CONSOLE_VAR_TYPE_STRING, value);
}


omeConsoleStatus omeConsoleRegisterVar(omeConsole *c, const char *name, omeConsoleVarType type, const void *value)
{
    omeConsoleVar *var;
    size_t size = strlen(name);

    if(size == 0 || size > OME_CONSOLE_MAX_VAR_NAME)
        return OME_CONSOLE_STATUS_INVALID_NAME;

    HASH_FIND_STR(c->variables, name, var);

    if(var != NULL)
        return OME_CONSOLE_STATUS_DUPLICATED_VARIABLE;

    var = omeConsoleVarCreate(name, type, value);
    HASH_ADD_STR(c->variables, key, var);

    return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleGetInt(omeConsole *c, const char *name, int *value)
{
    omeConsoleVar *var;

    var = omeConsoleGetVar(c, name);

    if(var == NULL)
        return OME_CONSOLE_STATUS_NOT_FOUND;

    if(var->type != OME_CONSOLE_VAR_TYPE_INT)
        return OME_CONSOLE_STATUS_BAD_TYPE;

    *value = var->value.intVal;

    return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleGetFloat(omeConsole *c, const char *name, float *value)
{
    omeConsoleVar *var;

    var = omeConsoleGetVar(c, name);

    if(var == NULL)
        return OME_CONSOLE_STATUS_NOT_FOUND;

    if(var->type != OME_CONSOLE_VAR_TYPE_FLOAT)
        return OME_CONSOLE_STATUS_BAD_TYPE;

    *value = var->value.floatVal;

    return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleStatus omeConsoleGetString(omeConsole *c, const char *name, const char **value)
{
    omeConsoleVar *var;

    var = omeConsoleGetVar(c, name);

    if(var == NULL)
        return OME_CONSOLE_STATUS_NOT_FOUND;

    if(var->type != OME_CONSOLE_VAR_TYPE_STRING)
        return OME_CONSOLE_STATUS_BAD_TYPE;

    *value = var->value.strVal;

    return OME_CONSOLE_STATUS_NO_ERROR;
}


omeConsoleVar *omeConsoleGetVar(omeConsole *c, const char *name)
{
    omeConsoleVar *var;

    HASH_FIND_STR(c->variables, name, var);
    return var;
}


omeConsoleStatus omeConsoleSetInt(omeConsole *c, const char *name, int value)
{
    return omeConsoleSetVar(c, name, OME_CONSOLE_VAR_TYPE_INT, &value);
}


omeConsoleStatus omeConsoleSetFloat(omeConsole *c, const char *name, float value)
{
    return omeConsoleSetVar(c, name, OME_CONSOLE_VAR_TYPE_FLOAT, &value);
}


omeConsoleStatus omeConsoleSetString(omeConsole *c, const char *name, const char *value)
{
    return omeConsoleSetVar(c, name, OME_CONSOLE_VAR_TYPE_STRING, value);
}


omeConsoleStatus omeConsoleSetVar(omeConsole *c, const char *name, omeConsoleVarType type, const void *value)
{
    omeConsoleVar *v;

    HASH_FIND_STR(c->variables, name, v);

    if(v != NULL)
        return OME_CONSOLE_STATUS_NOT_FOUND;

    if(v->type != type)
        return OME_CONSOLE_STATUS_BAD_TYPE;

    switch(type)
    {
        case OME_CONSOLE_VAR_TYPE_INT:
        case OME_CONSOLE_VAR_TYPE_BOOL:
        case OME_CONSOLE_VAR_TYPE_FLOAT:
            // TODO: works but ugly, get rid of this
            memcpy(&v->value, value, sizeof(int));
            break;
        case OME_CONSOLE_VAR_TYPE_STRING:
            strncpy(v->value.strVal, value, OME_CONSOLE_MAX_LINE_SIZE);
            v->value.strVal[OME_CONSOLE_MAX_LINE_SIZE - 1] = '\0';
            break;
    }

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
    static const char *errMsgs[OME_MAX_CONSOLE_STATUS + 1] =
    {
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
        "Unable to convert value",
        "Invalid identifier",

        "Unknown error"
    };

    if(status >= OME_MAX_CONSOLE_STATUS)
        status = OME_MAX_CONSOLE_STATUS;

    return errMsgs[status];
}


omeConsoleVar *omeConsoleVarCreate(const char *name, omeConsoleVarType type, const void *value)
{
    omeConsoleVar *var = calloc(1, sizeof(omeConsoleVar));

    strncpy(var->key, name, OME_CONSOLE_MAX_VAR_NAME);
    var->key[OME_CONSOLE_MAX_VAR_NAME - 1] = '\0';
    var->type = type;

    switch(type)
    {
        case OME_CONSOLE_VAR_TYPE_INT: var->value.intVal = *(int *)value; break;
        case OME_CONSOLE_VAR_TYPE_BOOL: var->value.boolVal = *(int *)value; break;
        case OME_CONSOLE_VAR_TYPE_FLOAT: var->value.floatVal = *(float *)value; break;
        case OME_CONSOLE_VAR_TYPE_STRING:
            strncpy(var->value.strVal, value, OME_CONSOLE_MAX_LINE_SIZE);
            var->value.strVal[OME_CONSOLE_MAX_LINE_SIZE - 1] = '\0';
            break;
    }

    return var;
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
        c->printCb("int %s = %d\n", v->key, v->value.intVal);
        break;
    case OME_CONSOLE_VAR_TYPE_FLOAT:
        c->printCb("float %s = %f\n", v->key, v->value.floatVal);
        break;
    case OME_CONSOLE_VAR_TYPE_STRING:
        c->printCb("string %s = \"%s\"\n", v->key, v->value.strVal);
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
