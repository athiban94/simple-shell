#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sysexits.h>
#include <stdbool.h>
#include <fcntl.h>

#define BUFFERSIZE 1024

int     exit_status; /* variable to store exit status */
int     original_fd_out;
int     original_fd_in;
int     trace;

char*   getinput(char *buffer, size_t buflen);
void    sig_int(int);
int     tokenize(char *str, char **tokenArray, int *length);
bool    handle_builtin(char **tokenArray, int len);
void    perform_echo(char **tokenArray, int len);
bool    is_builtin(char **tokenArray, int len);
int     fork_exec(char **tokenArray, int len);
void    execute_buf(char *buf, char **tokenArray, int *length);
int     tokenize_pipe_cmds(char  *str, char **tokenArray, int  *length);