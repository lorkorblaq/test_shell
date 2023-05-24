#ifndef _SHELL_H_
#define _SHELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>

/*buffer defined variable */
#define READ_BUF_SIZE 1024
#define WRITE_BUF_SIZE 1024
#define BUF_FLUSH -1

/* chain in command */
#define CMD_NORM	0
#define CMD_OR		1
#define CMD_AND		2
#define CMD_CHAIN	3

/* number_convert() */
#define CONVERT_LOWERCASE	1
#define CONVERT_UNSIGNED	2

/* 1 for getline() system */
#define USE_GETLINE 0
#define USE_STRTOK 0

#define HIST_FILE	".simple_shell_history"
#define HIST_MAX	4096

extern char **environ;
/**
 * num the number field
 * strt this is a string
 * points to the next node
 */
typedef struct liststr
{
	int num;
	char *str;
	struct liststr *next;
} list_t;
/**
 *struct infoParser - contains pseudo-arguements to pass into a function,
 *@environ,copy of environ from LL env
 *@status, the return status of the last exec'd command
 *@cmd_buf, address of pointer to cmd_buf, on if chaining
 *@argc, argument count
 *@line_count, the error count
 *@err_num,the error code for exit()s
 *@arg, a string generated from getline containing arguements
 *@linecount_flag, if on count this line of input
 *@cmd_buf_type, CMD_type or, and, ;
 *@readfd, the fd from which to read line input
 *@fname, the name of theprogram filename
 *@history, history node
 *@alias, alias node
 *@env_changed, on if environ was changed
 *@argv, an array of strings generated from arg
 *@path, a string path for the current command
 *@env, linked list local copy of environ
 */
typedef struct infoParser
{
	char *arg;
	char **argv;
	char *path;
	int argc;
	unsigned int line_count;
	int err_num;
	int linecount_flag;
	char *fname;
	list_t *env;
	list_t *history;
	list_t *alias;
	char **environ;
	int env_changed;
	int status;

	char **cmd_buf; /*chain buffer, for memory mangement, pointer to commmand */
	int cmd_buf_type; 
	int readfd;
	int histcount;
} data_p;

#define INFO_INIT \
{NULL, NULL, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, 0, 0, NULL, \
	0, 0, 0}

/**
 *struct contains a related function and a string of bulltein
 *@type, builtinto command flag
 *@func, function
 */
typedef struct builtinto
{
	char *type;
	int (*func)(data_p *);
} builtinto_table;


/* toem_shloop.c */
int hsh(data_p *, char **);
int builtIntoFinder(data_p *);
void commandFinder(data_p *);
void fork_cmd(data_p *);

/* toem_parser.c */
int is_cmd(data_p *, char *);
char *dup_chars(char *, int, int);
char *find_path(data_p *, char *, char *);

/* loophsh.c */
int loophsh(char **);

/* _errors.c */
void _eputs(char *);
int _eputchar(char);
int _putfd(char c, int fd);
int _putsfd(char *str, int fd);

/* _string.c */
int _strlen(char *);
int _strcmp(char *, char *);
char *starts_with(const char *, const char *);
char *_strcat(char *, char *);

/* _string1.c */
char *_strcpy(char *, char *);
char *_strdup(const char *);
void _puts(char *);
int _putchar(char);

/* _exits.c */
char *_strncpy(char *, char *, int);
char *_strncat(char *, char *, int);
char *_strchr(char *, char);

/* _tokenizer.c */
char **strtow(char *, char *);
char **strtow2(char *, char);

/* _realloc.c */
char *_memset(char *, char, unsigned int);
void ffree(char **);
void *_realloc(void *, unsigned int, unsigned int);

/* _memory.c */
int bfree(void **);

/* _atoi.c */
int interactive(data_p *);
int is_delim(char, char *);
int _isalpha(int);
int _atoi(char *);

/* _errors1.c */
int _erratoi(char *);
void print_error(data_p *, char *);
int print_d(int, int);
char *convert_number(long int, int, int);
void remove_comments(char *);

/* _builtinto.c */
int _myexit(data_p *);
int _mycd(data_p *);
int _myhelp(data_p *);

/* _builtinto1.c */
int _myhistory(data_p *);
int _myalias(data_p *);

/*_getline.c */
ssize_t get_input(data_p *);
int _getline(data_p *, char **, size_t *);
void sigintHandler(int);

/* _getinfo.c */
void clear_info(data_p *);
void set_info(data_p *, char **);
void free_info(data_p *, int);

/* _environ.c */
char *_getenv(data_p *, const char *);
int myEnv(data_p *);
int mySetEnv(data_p *);
int myUnSetEnv(data_p *);
int populate_env_list(data_p *);

/* _getenv.c */
char **getEnviron(data_p *);
int environUnSet(data_p *, char *);
int environSet(data_p *, char *, char *);

/* _history.c */
char *historyFileGotten(data_p *info);
int historyWritten(data_p *info);
int HistoryRead(data_p *info);
int listBuiltH(data_p *info, char *buf, int linecount);
int reNumHist(data_p *info);

/* _lists.c */
list_t *NodeAdded(list_t **, const char *, int);
list_t *NodeEndAdded(list_t **, const char *, int);
size_t strListP(const list_t *);
int delete_indexNode(list_t **, unsigned int);
void free_list(list_t **);

/* _lists1.c */
size_t list_len(const list_t *);
char **list_to_strings(list_t *);
size_t print_list(const list_t *);
list_t *startNodeW(list_t *, char *, char);
ssize_t getIndexNode(list_t *, list_t *);

/* _vars.c */
int chainIsk(data_p *, char *, size_t *);
void chainCheck(data_p *, char *, size_t *, size_t, size_t);
int alias_replace(data_p *);
int vars_replace(data_p *);
int string_replace(char **, char *);

#endif
