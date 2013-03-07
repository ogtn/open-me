//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#include "interp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


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


#pragma GCC diagnostic pop


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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


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


#pragma GCC diagnostic pop


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
