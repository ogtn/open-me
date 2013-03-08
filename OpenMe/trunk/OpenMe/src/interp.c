//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "interp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


cmd_status cmd_env_create_var(cmd_env *env, const cmd_var *var)
{
	if(env->varsNb >= CMD_MAX_VARS)
		return CMD_STATUS_ENV_FULL;

	env->vars[env->varsNb] = *var;
	env->varsNb++;

	return CMD_STATUS_NO_ERROR;
}


cmd_status cmd_env_get_var(cmd_env *env, const char *name, cmd_var **res)
{
	int i;

	for(i = 0; i < env->varsNb; i++)
	{
		if(strcmp(name, env->vars[i].name) == 0)
		{
			*res = &env->vars[i];

			return CMD_STATUS_NO_ERROR;
		}
	}

	*res = NULL;

	return CMD_STATUS_NOT_FOUND;
}


cmd_status cmd_env_set_var(cmd_env *env, const char *name, const char *value)
{
	cmd_var *var;

	if(cmd_env_get_var(env, name, &var) == CMD_STATUS_NOT_FOUND)
		return CMD_STATUS_NOT_FOUND;

	switch(var->type)
	{
		case CMD_VAR_TYPE_INT:
			if(sscanf(value, "%i", &var->data.i) != 1)
				return CMD_STATUS_CANT_CONVERT;
			break;
		case CMD_VAR_TYPE_FLOAT:
			if(sscanf(value, "%f", &var->data.f) != 1)
				return CMD_STATUS_CANT_CONVERT;
			break;
		case CMD_VAR_TYPE_STRING:
			if(strlen(value) == 0)
				return CMD_STATUS_CANT_CONVERT;
			strncpy(var->data.str, value, CMD_MAX_NAME);
			break;
		case CMD_VAR_TYPE_INT_PTR:
			if(sscanf(value, "%i", var->data.i_ptr) != 1)
				return CMD_STATUS_CANT_CONVERT;
			break;
		case CMD_VAR_TYPE_FLOAT_PTR:
			if(sscanf(value, "%f", var->data.f_ptr) != 1)
				return CMD_STATUS_CANT_CONVERT;
			break;
		default:
			return CMD_STATUS_BAD_TYPE;
	};

	return CMD_STATUS_NO_ERROR;
}


cmd_status cmd_env_get_cmd(cmd_env *env, const char *name, cmd_command **res)
{
	int i;

	for(i = 0; i < env->commandsNb; i++)
	{
		cmd_command *cmd = &env->commands[i];

		if(strcmp(cmd->name, name) == 0)
		{
			*res = cmd;

			return CMD_STATUS_NO_ERROR;
		}
	}

	*res = NULL;

	return CMD_STATUS_NOT_FOUND;
}


void cmd_env_clean(cmd_env *env)
{
	env->varsNb = 0;
}


void cmd_env_list_vars(const cmd_env *env)
{
	int i;

	for(i = 0; i < env->varsNb; i++)
		cmd_var_print(&env->vars[i]);
}


cmd_status cmd_env_register_cb(cmd_env *env, cmd_callback cb, const char *name, void  *user_data)
{
	cmd_command *command;

	if(env->commandsNb >= CMD_MAX_COMMANDS)
		return CMD_STATUS_ENV_FULL;

	if(cb == NULL)
		return CMD_STATUS_INVALID_CB;

	if(cmd_env_get_cmd(env, name, &command) != CMD_STATUS_NOT_FOUND)
		return CMD_STATUS_DUPLICATED_COMMAND;

	command = &env->commands[env->commandsNb];
	command->cb = cb;
	strncpy(command->name, name, CMD_MAX_NAME);
	command->user_data = user_data;
	env->commandsNb++;

	return CMD_STATUS_NO_ERROR;
}


void cmd_env_print(cmd_env *env)
{
	int i;

	printf("%d variable(s):\n", env->varsNb);

	for(i = 0; i < env->varsNb; i++)
		cmd_var_print(&env->vars[i]);

	printf("%d command(s):\n", env->commandsNb);

	for(i = 0; i < env->commandsNb; i++)
		printf("'%s'()\n", env->commands[i].name);
}


cmd_status cmd_var_print(const cmd_var *var)
{
	switch(var->type)
	{
		case CMD_VAR_TYPE_INT:
			printf("int %s = %d\n", var->name, var->data.i);
			/*printf("Var '%s' of type int, value = '%d'\n", var->name, var->data.i);*/
			break;
		case CMD_VAR_TYPE_FLOAT:
			printf("float %s = %f\n", var->name, var->data.f);
			/*printf("Var '%s' of type float, value = '%f'\n", var->name, var->data.f);*/
			break;
		case CMD_VAR_TYPE_STRING:
			printf("string %s = \"%s\"\n", var->name, var->data.str);
			/*printf("Var '%s' of type string, value = '%s'\n", var->name, var->data.str);*/
			break;
		case CMD_VAR_TYPE_INT_PTR:
			printf("int *%s = %d\n", var->name, *var->data.i_ptr);
			/*printf("Var '%s' of type int, value = '%d'\n", var->name, *var->data.i_ptr);*/
			break;
		case CMD_VAR_TYPE_FLOAT_PTR:
			printf("float *%s = %f\n", var->name, *var->data.f_ptr);
			/*printf("Var '%s' of type float, value = '%f'\n", var->name, *var->data.f_ptr);*/
			break;
		default:
			return CMD_STATUS_BAD_TYPE;
	};

	return CMD_STATUS_NO_ERROR;
}


cmd_status cmd_line_parse(const char *str, cmd_line *cmdline)
{
	memset(cmdline, 0, sizeof(*cmdline));
	/*cmdline->argsNb = 0;*/

	cmd_skip_blanks(&str);

	if(sscanf(str, "%s", cmdline->cmd) == 1)
		str += strlen(cmdline->cmd);

	while(*str)
	{
		if(cmd_is_blank(*str))
		{
			cmd_skip_blanks(&str);
			continue;
		}

		if(*str == '"')
		{
			int j = 0;
			str++;

			for(j = 0; *str && *str != '"' && j < CMD_MAX_NAME; j++, str++)
				cmdline->args[cmdline->argsNb][j] = *str;

			cmdline->argsNb++;

			if(*str == '"')
				str++;
			
			continue;
		}

		if(sscanf(str, "%s", cmdline->args[cmdline->argsNb]) == 1)
		{
			str += strlen(cmdline->args[cmdline->argsNb]);
			cmdline->argsNb++;

			if(cmdline->argsNb >= CMD_MAX_ARGS)
				return CMD_STATUS_TOO_MUCH_ARGS;
		}
		else
		{
			printf("##shit happened\n");
			str++;
		}
	}

	return CMD_STATUS_NO_ERROR;
}


cmd_status cmd_line_interpret(const cmd_line *cmdline, cmd_env *env)
{
	cmd_var *var;
	cmd_command *cmd;

	if(cmd_env_get_cmd(env, cmdline->cmd, &cmd) != CMD_STATUS_NOT_FOUND)
	{
		cmd->cb(cmdline, env, cmd->user_data);

		return CMD_STATUS_NO_ERROR;
	}

	if(cmd_env_get_var(env, cmdline->cmd, &var) != CMD_STATUS_NOT_FOUND)
	{
		cmd_var_print(var);

		return CMD_STATUS_NO_ERROR;
	}

	return CMD_STATUS_INVALID_CMD;
}


void cmd_line_clean(char *str)
{
	while(*str)
	{
		if(*str == '\n' || *str == '\r')
		{
			*str = '\0';
			return;
		}

		str++;
	}
}


void cmd_line_print(const cmd_line *cmdline)
{
	int i;

	printf("cmd : '%s'\n", cmdline->cmd);

	for(i = 0; i < cmdline->argsNb; i++)
		printf("arg %d: '%s'\n", i, cmdline->args[i]);
}


int cmd_is_blank(char c)
{
	return c == ' ';
}


void cmd_skip_blanks(const char **str)
{
	while(**str && cmd_is_blank(**str))
		(*str)++;
}
