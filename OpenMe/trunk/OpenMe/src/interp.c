//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define CMD_MAX_VARS			8
#define CMD_MAX_FUNCS			4

#define CMD_MAX_NAME			32
#define CMD_MAX_LINE_SIZE		256
#define CMD_MAX_ARGS			4

typedef enum cmd_status
{
	CMD_STATUS_NO_ERROR,
	CMD_STATUS_BAD_TYPE,
	CMD_STATUS_ENV_FULL,
	CMD_STATUS_NOT_FOUND,
	CMD_STATUS_TOO_MUCH_ARGS
} cmd_status;

typedef enum cmd_var_type
{
	CMD_VAR_TYPE_INT,
	CMD_VAR_TYPE_FLOAT
	/* CMD_VAR_TYPE_STRING */
} cmd_var_type;

typedef struct cmd_var
{
	char name[CMD_MAX_NAME];
	cmd_var_type type;
	union
	{
		int i;
		float f;
		/* char *str; */
	} data;
} cmd_var;

typedef struct cmd_func
{
	char name[CMD_MAX_NAME];
} cmd_func;

typedef struct cmd_env
{
	int varsNb;
	cmd_var vars[CMD_MAX_VARS];
	int funcNb;
	cmd_func funcs[CMD_MAX_FUNCS];
} cmd_env;

typedef struct cmd_line
{
	int argsNb;
	char cmd[CMD_MAX_NAME];
	char args[CMD_MAX_ARGS][CMD_MAX_NAME];
} cmd_line;

/* env functions */
cmd_status cmd_env_create_var(cmd_env *env, const cmd_var *var);
cmd_status cmd_env_get_var(cmd_env *env, const char *name, cmd_var **res);
void cmd_env_clean(cmd_env *env);
void cmd_env_list_vars(const cmd_env *env);

/* var functions */
cmd_status cmd_var_print(const cmd_var *var);

/* line functions */
cmd_status cmd_line_parse(const char *str, cmd_line *line);
void cmd_line_clean(char *str);
void cmd_line_print(const cmd_line *cmdline);

/* utils */
int cmd_is_blank(char c);
void cmd_skip_blanks(const char **str);

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

cmd_status cmd_var_print(const cmd_var *var)
{
	switch(var->type)
	{
		case CMD_VAR_TYPE_INT:
			printf("Var '%s' of type int, value = '%d'\n", var->name, var->data.i);
			break;
		case CMD_VAR_TYPE_FLOAT:
			printf("Var '%s' of type float, value = '%f'\n", var->name, var->data.f);
			break;
		/*case CMD_VAR_TYPE_STRING:
			printf("Var '%s' of type string, value = '%s'\n", var->name, var->data.str);
			break;*/
		default:
			return CMD_STATUS_BAD_TYPE;
	};

	return CMD_STATUS_NO_ERROR;
}

static void tests(cmd_env *env)
{
	cmd_var test_int, test_float;
	cmd_var *res;

	/* create and add 2 vars to the environment */
	{
		strncpy(test_int.name, "test_int", CMD_MAX_NAME);
		test_int.type = CMD_VAR_TYPE_INT;
		test_int.data.i = 42;
		cmd_env_create_var(env, &test_int);

		strncpy(test_float.name, "test_float", CMD_MAX_NAME);
		test_float.type = CMD_VAR_TYPE_FLOAT;
		test_float.data.f = 1.333;
		cmd_env_create_var(env, &test_float);
	}

	/* print them */
	{
		assert(cmd_var_print(&test_int) == CMD_STATUS_NO_ERROR);
		assert(cmd_var_print(&test_float) == CMD_STATUS_NO_ERROR);
	}

	/* retreive the vars from the environment */
	{
		assert(cmd_env_get_var(env, "test_int", &res) == CMD_STATUS_NO_ERROR);
		assert(memcmp(&test_int, res, sizeof(cmd_var)) == 0);

		assert(cmd_env_get_var(env, "test_float", &res) == CMD_STATUS_NO_ERROR);
		assert(memcmp(&test_float, res, sizeof(cmd_var)) == 0);

		assert(cmd_env_get_var(env, "crazy undefined var name", &res) == CMD_STATUS_NOT_FOUND);
	}
}

cmd_status cmd_line_parse(const char *str, cmd_line *cmdline)
{
	int res;

	memset(cmdline, 0, sizeof(*cmdline));
	/*cmdline->argsNb = 0;*/

	cmd_skip_blanks(&str);
	res = sscanf(str, "%s", /*CMD_MAX_NAME,*/ cmdline->cmd);

	if(res == 1)
	{
		str += strlen(cmdline->cmd);
		/*printf("##found cmd '%s', remains: '%s'\n", cmdline->cmd, str);*/
	}

	while(*str)
	{
		if(cmd_is_blank(*str))
		{
			/*cmdline->argsNb++;*/
			cmd_skip_blanks(&str);
			/*printf("##found blanks, remains: '%s'\n", str);*/
		}

		res = sscanf(str, "%s", /*CMD_MAX_NAME,*/ cmdline->args[cmdline->argsNb]);

		if(res == 1)
		{
			str += strlen(cmdline->args[cmdline->argsNb]);
			/*printf("##found arg%d '%s', remains: '%s'\n", cmdline->argsNb, cmdline->args[cmdline->argsNb], str);*/
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

	printf("== cmd line ==\n");

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

int main(void)
{
	char line[CMD_MAX_LINE_SIZE] = {'\0'};
	char name[CMD_MAX_LINE_SIZE] = {'\0'};
	char cmd[CMD_MAX_LINE_SIZE] = {'\0'};
	static cmd_env env;
	cmd_line cmdline;
	cmd_var *var;

	tests(&env);

	do
	{
		printf("> ");
		fgets(line, CMD_MAX_LINE_SIZE, stdin);
		cmd_line_clean(line);
		cmd_line_parse(line, &cmdline);
		cmd_line_print(&cmdline);

		/*
		if(sscanf(line, "%s", cmd) == 1)
		{
			printf("command '%s'\n", cmd);

			if(strcmp(cmd, "get") == 0)
			{
				if(cmd_env_get_var(&env, name, &var) == CMD_STATUS_NOT_FOUND)
					printf("Variable '%s' doesn't exist in this environment\n", name);
				else
					cmd_var_print(var);
			}
			else if(strcmp(cmd, "set") == 0)
			{
				if(cmd_env_get_var(&env, name, &var) == CMD_STATUS_NOT_FOUND)
					printf("Variable '%s' doesn't exist in this environment\n", name);
				else
					printf("Not implemented yet\n");
			}
			else if(strcmp(cmd, "quit") == 0)
			{
				printf("Exiting interpreter\n");
			}
			else
			{
				printf("Invalid command: '%s'\n", line);
			}

		}*/
	} while(strcmp(cmdline.cmd, "q"));

	return EXIT_SUCCESS;
}
