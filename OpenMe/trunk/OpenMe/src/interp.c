/*
//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define CMD_MAX_VARS			8
#define CMD_MAX_COMMANDS		16
#define CMD_MAX_NAME			32
#define CMD_MAX_LINE_SIZE		256
#define CMD_MAX_ARGS			8

typedef enum cmd_status
{
	CMD_STATUS_NO_ERROR,
	CMD_STATUS_BAD_TYPE,
	CMD_STATUS_ENV_FULL,
	CMD_STATUS_NOT_FOUND,
	CMD_STATUS_INVALID_CB,
	CMD_STATUS_TOO_MUCH_ARGS,
	CMD_STATUS_DUPLICATED_COMMAND,
	CMD_STATUS_INVALID_CMD,
	CMD_STATUS_CANT_CONVERT
} cmd_status;

typedef enum cmd_var_type
{
	CMD_VAR_TYPE_INT,
	CMD_VAR_TYPE_FLOAT,
	CMD_VAR_TYPE_STRING,
	CMD_VAR_TYPE_INT_PTR,
	CMD_VAR_TYPE_FLOAT_PTR
} cmd_var_type;

typedef struct cmd_var
{
	char name[CMD_MAX_NAME];
	cmd_var_type type;
	union
	{
		int i;
		float f;
		char *str;
		int *i_ptr;
		float *f_ptr;
	} data;
} cmd_var;

typedef struct cmd_line
{
	int argsNb;
	char cmd[CMD_MAX_NAME];
	char args[CMD_MAX_ARGS][CMD_MAX_NAME];
} cmd_line;

typedef struct cmd_env cmd_env;
typedef void(*cmd_callback)(const cmd_line *cmdline, cmd_env *env, void *data);

typedef struct cmd_command
{
	cmd_callback cb;
	char name[CMD_MAX_NAME];
	void *user_data;
} cmd_command;

struct cmd_env
{
	int varsNb;
	cmd_var vars[CMD_MAX_VARS];
	int commandsNb;
	cmd_command commands[CMD_MAX_COMMANDS];
};

/* env functions */
cmd_status cmd_env_create_var(cmd_env *env, const cmd_var *var);
cmd_status cmd_env_get_var(cmd_env *env, const char *name, cmd_var **res);
cmd_status cmd_env_set_var(cmd_env *env, const char *name, const char *value);
cmd_status cmd_env_get_cmd(cmd_env *env, const char *name, cmd_command **res);
void cmd_env_clean(cmd_env *env);
void cmd_env_list_vars(const cmd_env *env);
cmd_status cmd_env_register_cb(cmd_env *env, cmd_callback cb, const char *name, void  *user_data);
void cmd_env_print(cmd_env *env);

/* var functions */
cmd_status cmd_var_print(const cmd_var *var);

/* line functions */
cmd_status cmd_line_parse(const char *str, cmd_line *cmdline);
cmd_status cmd_line_interpret(const cmd_line *cmdline, cmd_env *env);
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

/* Tests */
static void test_cb_1(const cmd_line *cmdline, cmd_env *env, void *data)
{
	assert(cmdline != NULL);
	assert(env != NULL);
	assert(data != NULL);
	assert(*(int *)data == 123456789);

	*(int *)data = 42;
	puts("test_cb_1()");
}

static void test_cb_2(const cmd_line *cmdline, cmd_env *env, void *data)
{
	assert(cmdline != NULL);
	assert(env != NULL);
	assert(data != NULL);
	assert(*(int *)data == 42);

	*(int *)data = 666;
	puts("test_cb_2()");
}

static void test_cb_3(const cmd_line *cmdline, cmd_env *env, void *data)
{
	assert(cmdline != NULL);
	assert(env != NULL);
	assert(data == NULL);

	puts("test_cb_3()");
}

static void test_main(void)
{
	char line[CMD_MAX_LINE_SIZE] = {'\0'};
	cmd_var test_int, test_float;
	cmd_line cmdline;
	cmd_var *res;
	static cmd_env env; /* TODO: provide a cmd_env_create function to avoid the need of static initialization */

	puts("=====[begin tests]=====");

	/* create and add vars to the environment */
	{
		strncpy(test_int.name, "test_int", CMD_MAX_NAME);
		test_int.type = CMD_VAR_TYPE_INT;
		test_int.data.i = 42;
		cmd_env_create_var(&env, &test_int);

		/* //cmd_env_create_var_int(&env, "test_int", 42); */

		strncpy(test_float.name, "test_float", CMD_MAX_NAME);
		test_float.type = CMD_VAR_TYPE_FLOAT;
		test_float.data.f = 1.333;
		cmd_env_create_var(&env, &test_float);
	}

	/* print them */
	{
		assert(cmd_var_print(&test_int) == CMD_STATUS_NO_ERROR);
		assert(cmd_var_print(&test_float) == CMD_STATUS_NO_ERROR);
	}

	/* retreive the vars from the environment */
	{
		assert(cmd_env_get_var(&env, "test_int", &res) == CMD_STATUS_NO_ERROR);
		assert(memcmp(&test_int, res, sizeof(cmd_var)) == 0);

		assert(cmd_env_get_var(&env, "test_float", &res) == CMD_STATUS_NO_ERROR);
		assert(memcmp(&test_float, res, sizeof(cmd_var)) == 0);

		assert(cmd_env_get_var(&env, "crazy undefined var name", &res) == CMD_STATUS_NOT_FOUND);
	}

	/* create commands and register their associated callbacks */
	{
		int n = 123456789;

		strncpy(line, "cb_1", CMD_MAX_LINE_SIZE);
		assert(cmd_line_parse(line, &cmdline) == CMD_STATUS_NO_ERROR);
		assert(cmd_line_interpret(&cmdline, &env) == CMD_STATUS_INVALID_CMD);
		
		assert(cmd_env_register_cb(&env, test_cb_1, "cb_1", &n) == CMD_STATUS_NO_ERROR);
		assert(cmd_line_parse(line, &cmdline) == CMD_STATUS_NO_ERROR);
		assert(cmd_line_interpret(&cmdline, &env) == CMD_STATUS_NO_ERROR);

		assert(cmd_env_register_cb(&env, test_cb_1, "cb_1", &n) == CMD_STATUS_DUPLICATED_COMMAND);
		assert(cmd_env_register_cb(&env, test_cb_2, "cb_2", &n) == CMD_STATUS_NO_ERROR);
		assert(cmd_env_register_cb(&env, test_cb_3, "cb_3", NULL) == CMD_STATUS_NO_ERROR);

		strncpy(line, "cb_2", CMD_MAX_LINE_SIZE);
		assert(cmd_line_parse(line, &cmdline) == CMD_STATUS_NO_ERROR);
		assert(cmd_line_interpret(&cmdline, &env) == CMD_STATUS_NO_ERROR);

		strncpy(line, "cb_3", CMD_MAX_LINE_SIZE);
		assert(cmd_line_parse(line, &cmdline) == CMD_STATUS_NO_ERROR);
		assert(cmd_line_interpret(&cmdline, &env) == CMD_STATUS_NO_ERROR);
	}

	puts("=====[end tests]=====");
}

static void cb_quit(const cmd_line *cmdline, cmd_env *env, void *user_data)
{
	*(int *)user_data = 1;
	printf("Bye, cruel world!\n");
}

static void cb_get(const cmd_line *cmdline, cmd_env *env, void *user_data)
{
	const char *name = cmdline->args[0];
	cmd_var *var;

	if(cmd_env_get_var(env, name, &var) == CMD_STATUS_NOT_FOUND)
		printf("Variable '%s' doesn't exist in this environment\n", name);
	else
		cmd_var_print(var);	
}

static void cb_set(const cmd_line *cmdline, cmd_env *env, void *user_data)
{
	const char *name = cmdline->args[0];
	cmd_var *var;

	if(cmd_env_get_var(env, name, &var) == CMD_STATUS_NOT_FOUND)
		printf("Variable '%s' doesn't exist in this environment\n", name);
	else
		cmd_env_set_var(env, cmdline->args[0], cmdline->args[1]);
}

static void cb_list(const cmd_line *cmdline, cmd_env *env, void *user_data)
{
	cmd_env_print(env);
}

static void cb_create(const cmd_line *cmdline, cmd_env *env, void *user_data)
{
	cmd_var var;

	if(cmdline->argsNb < 2)
	{
		printf("command create failed: missing arguments\n");
		return;
	}

	if(strcmp(cmdline->args[0], "int") == 0)
	{
		var.type = CMD_VAR_TYPE_INT;
		var.data.i = 0;
	}
	else if(strcmp(cmdline->args[0], "float") == 0)
	{
		var.type = CMD_VAR_TYPE_FLOAT;
		var.data.f = 0.f;
	}
	else
	{
		printf("command create failed: invalid type specified\n");
		return;
	}

	strncpy(var.name, cmdline->args[1], CMD_MAX_NAME);
	cmd_env_create_var(env, &var);
}

int main(void)
{
	char line[CMD_MAX_LINE_SIZE] = {'\0'};
	static cmd_env env; /* TODO: provide a cmd_env_create function to avoid the need of static initialization */
	cmd_line cmdline;
	int stop = 0;
	cmd_var var;
	char titi[CMD_MAX_NAME] = "Coucou";

	test_main();

	/* setting env variables */
	strncpy(var.name, "toto", CMD_MAX_NAME);
	var.type = CMD_VAR_TYPE_INT;
	var.data.i = 42;
	cmd_env_create_var(&env, &var);

	strncpy(var.name, "tata", CMD_MAX_NAME);
	var.type = CMD_VAR_TYPE_FLOAT;
	var.data.f = 123.456f;
	cmd_env_create_var(&env, &var);

	strncpy(var.name, "titi", CMD_MAX_NAME);
	var.type = CMD_VAR_TYPE_STRING;
	var.data.str = titi;
	cmd_env_create_var(&env, &var);

	/* setting env commands */
	cmd_env_register_cb(&env, cb_quit, "q", &stop);
	cmd_env_register_cb(&env, cb_quit, "quit", &stop);
	cmd_env_register_cb(&env, cb_get, "get", NULL);
	cmd_env_register_cb(&env, cb_set, "set", NULL);
	cmd_env_register_cb(&env, cb_list, "list", NULL);
	cmd_env_register_cb(&env, cb_create, "create", NULL);

	while(!stop)
	{
		printf("> ");
		
		if(fgets(line, CMD_MAX_LINE_SIZE, stdin) == NULL)
			printf("Weird stuff happened\n");

		cmd_line_clean(line);
		cmd_line_parse(line, &cmdline);
		
		if(cmd_line_interpret(&cmdline, &env) == CMD_STATUS_INVALID_CMD)
		{
			printf("Invalid command:\n");
			cmd_line_print(&cmdline);
		}
	}

	return EXIT_SUCCESS;
}
