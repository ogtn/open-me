//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef OME_INTERP_H
#define OME_INTERP_H


#ifdef __cplusplus
extern "C" {
#endif


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


#ifdef __cplusplus
}
#endif


#endif // OME_INTERP_H
