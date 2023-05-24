#include "shell.h"

/**
 * input_buf 
 * @info the parameter for struct
 * @buf the address of the buffer allocated
 * @len lentgh of the address of the 
 */
ssize_t input_buf(data_p *info, char **buf, size_t *len)
{
	ssize_t char_r = 0;
	size_t len_path= 0;

	if (!*len) /* if buffer has nothing, then go ahead to fill it */
	{
		free(*buf);
		*buf = NULL;
		signal(SIGINT, sigintHandler);
#if USE_GETLINE
		char_r = getline(buf, &len_path, stdin);
#else
		char_r = _getline(info, buf, &len_path);
#endif
		if (char_r > 0)
		{
			if ((*buf)[char_r - 1] == '\n')
			{
				(*buf)[char_r - 1] = '\0'; /* remove trailing newline */
				char_r--;
			}
			info->linecount_flag = 1;
			remove_comments(*buf);
			listBuiltH(info, *buf, info->histcount++);
			/* if (_strchr(*buf, ';')) is this a command chain? */
			{
				*len = char_r;
				info->cmd_buf = buf;
			}
		}
	}
	return (char_r);
}

/**
 * get_input - gets a line minus the newline
 * @info: parameter struct
 *
 * Return: bytes read
 */
ssize_t get_input(data_p *info)
{
	static char *buf; /* the ';' command chain buffer */
	static size_t i, j, len;
	ssize_t char_r = 0;
	char **buf_p = &(info->arg), *p;

	_putchar(BUF_FLUSH);
	char_r = input_buf(info, &buf, &len);
	if (char_r == -1) /* EOF */
		return (-1);
	if (len)	/* we have commands left in the chain buffer */
	{
		j = i; /* init new iterator to current buf position */
		p = buf + i; /* get pointer for return */

		chainCheck(info, buf, &j, i, len);
		while (j < len) /* iterate to semicolon or end */
		{
			if (chainIsk(info, buf, &j))
				break;
			j++;
		}

		i = j + 1; /* increment past nulled ';'' */
		if (i >= len) /* reached end of buffer? */
		{
			i = len = 0; /* reset position and length */
			info->cmd_buf_type = CMD_NORM;
		}

		*buf_p = p; /* pass back pointer to current command position */
		return (_strlen(p)); /* return length of current command */
	}

	*buf_p = buf; /* else not a chain, pass back buffer from _getline() */
	return (char_r); /* return length of buffer from _getline() */
}

/**
 * read_buf - reads a buffer
 * @info: parameter struct
 * @buf: buffer
 * @i: size
 *
 * Return: r
 */
ssize_t read_buf(data_p *info, char *buf, size_t *i)
{
	ssize_t char_r = 0;

	if (*i)
		return (0);
	char_r = read(info->readfd, buf, READ_BUF_SIZE);
	if (char_r >= 0)
		*i = char_r;
	return (char_r);
}

/**
 * _getline - gets the next line of input from STDIN
 * @info: parameter struct
 * @ptr: address of pointer to buffer, preallocated or NULL
 * @length: size of preallocated ptr buffer if not NULL
 *
 * Return: s
 */
int _getline(data_p *info, char **ptr, size_t *length)
{
	static char buf[READ_BUF_SIZE];
	static size_t i, len;
	size_t k;
	ssize_t char_r = 0, s = 0;
	char *p = NULL, *new_p = NULL, *c;

	p = *ptr;
	if (p && length)
		s = *length;
	if (i == len)
		i = len = 0;

	char_r = read_buf(info, buf, &len);
	if (char_r == -1 || (char_r == 0 && len == 0))
		return (-1);

	c = _strchr(buf + i, '\n');
	k = c ? 1 + (unsigned int)(c - buf) : len;
	new_p = _realloc(p, s, s ? s + k : k + 1);
	if (!new_p) /* MALLOC FAILURE! */
		return (p ? free(p), -1 : -1);

	if (s)
		_strncat(new_p, buf + i, k - i);
	else
		_strncpy(new_p, buf + i, k - i + 1);

	s += k - i;
	i = k;
	p = new_p;

	if (length)
		*length = s;
	*ptr = p;
	return (s);
}

/**
 * sigintHandler - blocks ctrl-C
 * @sig_num: the signal number
 *
 * Return: void
 */
void sigintHandler(__attribute__((unused))int sig_num)
{
	_puts("\n");
	_puts("$ ");
	_putchar(BUF_FLUSH);
}
