#include "shell.h"

/**
 * hsh - main shell loop
 * @info: the parameter & return info struct
 * @av: the argument vector from main()
 *
 * Return: 0 on success, 1 on error, or error code
 */
int hsh(data_p *info, char **av)
{
	ssize_t r = 0;
	int builtinto_ret = 0;

	while (r != -1 && builtinto_ret != -2)
	{
		clear_info(info);
		if (interactive(info))
			_puts("$ ");
		_eputchar(BUF_FLUSH);
		r = get_input(info);
		if (r != -1)
		{
			set_info(info, av);
			builtinto_ret = builtIntoFinder(info);
			if (builtinto_ret == -1)
				commandFinder(info);
		}
		else if (interactive(info))
			_putchar('\n');
		free_info(info, 0);
	}
	historyWritten(info);
	free_info(info, 1);
	if (!interactive(info) && info->status)
		exit(info->status);
	if (builtinto_ret == -2)
	{
		if (info->err_num == -1)
			exit(info->status);
		exit(info->err_num);
	}
	return (builtinto_ret);
}

/**
 * builtIntoFinder - finds a builtinto command
 * @info: the parameter & return info struct
 *
 * Return: -1 if builtinto not found,
 *			0 if builtinto executed successfully,
 *			1 if builtinto found but not successful,
 *			-2 if builtinto signals exit()
 */
int builtIntoFinder(data_p *info)
{
	int i, built_in_ret = -1;
	builtinto_table builtintotbl[] = {
		{"exit", _myexit},
		{"env", myEnv},
		{"help", _myhelp},
		{"history", _myhistory},
		{"setenv", mySetEnv},
		{"unsetenv", myUnSetEnv},
		{"cd", _mycd},
		{"alias", _myalias},
		{NULL, NULL}
	};

	for (i = 0; builtintotbl[i].type; i++)
		if (_strcmp(info->argv[0], builtintotbl[i].type) == 0)
		{
			info->line_count++;
			built_in_ret = builtintotbl[i].func(info);
			break;
		}
	return (built_in_ret);
}

/**
 * commandFinder - finds a command in PATH
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void commandFinder(data_p *info)
{
	char *path = NULL;
	int i, k;

	info->path = info->argv[0];
	if (info->linecount_flag == 1)
	{
		info->line_count++;
		info->linecount_flag = 0;
	}
	for (i = 0, k = 0; info->arg[i]; i++)
		if (!is_delim(info->arg[i], " \t\n"))
			k++;
	if (!k)
		return;

	path = find_path(info, _getenv(info, "PATH="), info->argv[0]);
	if (path)
	{
		info->path = path;
		fork_cmd(info);
	}
	else
	{
		if ((interactive(info) || _getenv(info, "PATH=")
			|| info->argv[0][0] == '/') && is_cmd(info, info->argv[0]))
			fork_cmd(info);
		else if (*(info->arg) != '\n')
		{
			info->status = 127;
			print_error(info, "not found\n");
		}
	}
}

/**
 * fork_cmd - forks a an exec thread to run cmd
 * @info: the parameter & return info struct
 *
 * Return: void
 */
void fork_cmd(data_p *info)
{
	pid_t child_pid;

	child_pid = fork();
	if (child_pid == -1)
	{
		/* TODO: PUT ERROR FUNCTION */
		perror("Error:");
		return;
	}
	if (child_pid == 0)
	{
		if (execve(info->path, info->argv, getEnviron(info)) == -1)
		{
			free_info(info, 1);
			if (errno == EACCES)
				exit(126);
			exit(1);
		}
		/* TODO: PUT ERROR FUNCTION */
	}
	else
	{
		wait(&(info->status));
		if (WIFEXITED(info->status))
		{
			info->status = WEXITSTATUS(info->status);
			if (info->status == 126)
				print_error(info, "Permission denied\n");
		}
	}
}
