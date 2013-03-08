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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

static void test_cb_1(omeConsole *c, void *data)
{
	assert(c != NULL);
	assert(data != NULL);
	assert(*(int *)data == 123456789);
	*(int *)data = 42;
}


static void test_cb_2(omeConsole *c, void *data)
{
	assert(c != NULL);
	assert(data != NULL);
	assert(*(int *)data == 42);
	*(int *)data = 666;
}


static void test_cb_3(omeConsole *c, void *data)
{
	assert(c != NULL);
	assert(data == NULL);
}

#pragma GCC diagnostic pop


static void test_main(void)
{
	omeConsole *c;
	
	int int_1 = 42;
	int int_2 = 666;
	
	float float_1 = 1.333f;
	float float_2 = 3.111f;

	char str_1[OME_CONSOLE_MAX_LINE_SIZE] = "abc";
	char str_2[OME_CONSOLE_MAX_LINE_SIZE] = "xyz";

	c = omeConsoleCreate();

	/* create and add vars to the environment */
	{
		assert(omeConsoleRegisterInt(c, "test_int", &int_1) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(omeConsoleRegisterFloat(c, "test_float", &float_1) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(omeConsoleRegisterString(c, "test_string", str_1) == OME_CONSOLE_STATUS_NO_ERROR);

		assert(omeConsoleRegisterInt(c, "test_int", &int_2) == OME_CONSOLE_STATUS_DUPLICATED_VARIABLE);
		assert(omeConsoleRegisterFloat(c, "test_float", &float_2) == OME_CONSOLE_STATUS_DUPLICATED_VARIABLE);
		assert(omeConsoleRegisterString(c, "test_string", str_2) == OME_CONSOLE_STATUS_DUPLICATED_VARIABLE);

		assert(omeConsoleRegisterInt(c, "", &int_2) == OME_CONSOLE_STATUS_INVALID_NAME);
		assert(omeConsoleRegisterFloat(c, "", &float_2) == OME_CONSOLE_STATUS_INVALID_NAME);
		assert(omeConsoleRegisterString(c, "", str_2) == OME_CONSOLE_STATUS_INVALID_NAME);
	}

	/* print them */
	{
		// TODO: implement this...
		// assert(omeConsoleVarPrint(c, "test_int") == CMD_STATUS_NO_ERROR);
		// assert(omeConsoleVarPrint(c, "test_float") == CMD_STATUS_NO_ERROR);
		// assert(omeConsoleVarPrint(c, "test_string") == CMD_STATUS_NO_ERROR);
	}

	/* retreive the vars from the environment */
	{
		assert(omeConsoleGetInt(c, "test_int", &int_2) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(int_1 == int_2);

		assert(omeConsoleGetFloat(c, "test_float", &float_2) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(float_1 == float_2);

		assert(omeConsoleGetString(c, "test_string", str_2) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(strcmp(str_1, str_2) == 0);

		assert(omeConsoleGetFloat(c, "test_int", &float_2) == OME_CONSOLE_STATUS_BAD_TYPE);
		assert(omeConsoleGetInt(c, "crazy undefined var name", &int_2) == OME_CONSOLE_STATUS_NOT_FOUND);
	}

	/* create commands and register their associated callbacks */
	{
		int n = 123456789;

		assert(omeConsoleProcess(c,
			"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
			aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
			aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
			aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			) == OME_CONSOLE_STATUS_LINE_OVERFLOW);

		assert(omeConsoleProcess(c, "cb_1") == OME_CONSOLE_STATUS_INVALID_CMD);
		
		assert(omeConsoleRegisterCallback(c, test_cb_1, "cb_1", &n) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(omeConsoleProcess(c, "cb_1") == OME_CONSOLE_STATUS_NO_ERROR);

		assert(omeConsoleRegisterCallback(c, test_cb_1, "cb_1", &n) == OME_CONSOLE_STATUS_DUPLICATED_COMMAND);
		assert(omeConsoleRegisterCallback(c, test_cb_2, "cb_2", &n) == OME_CONSOLE_STATUS_NO_ERROR);
		assert(omeConsoleRegisterCallback(c, test_cb_3, "cb_3", NULL) == OME_CONSOLE_STATUS_NO_ERROR);

		assert(omeConsoleProcess(c, "cb_2") == OME_CONSOLE_STATUS_NO_ERROR);
		assert(omeConsoleProcess(c, "cb_3") == OME_CONSOLE_STATUS_NO_ERROR);
	}

	omeConsoleDestroy(&c);
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*
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
*/

#pragma GCC diagnostic pop


int main(void)
{
	char line[OME_CONSOLE_MAX_LINE_SIZE] = {'\0'};
	char titi[OME_CONSOLE_MAX_VAR_NAME] = "Coucou";
	int stop = 0;

	test_main();
/*
	// setting env variables
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

	// setting env commands
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
*/
	return EXIT_SUCCESS;
}
