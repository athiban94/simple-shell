#include "sish.h"

/**
 * Function to print the shell prompt and get the string from the user.
 * */
char* 
getinput(char *buffer, size_t buflen) {
    printf("sish$ ");
    return fgets(buffer, buflen, stdin);
}

/**
 * Function to split the given string into tokens
 * */
int    
tokenize(char *str, char **tokenArray, int *length) {
    char    *token;
    char    *p;
    int final_output_fd, final_input_fd, fd, modified;

    final_output_fd = STDOUT_FILENO;
    final_input_fd  = STDIN_FILENO;

    if ((p = malloc(ARG_MAX)) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
        exit_status = 127;
	}

    modified = 0;

    /* spliting into tokens by space */
    token = strtok(str, " "); 
    
    while (token != NULL) {
    
        if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " ");

            if (token == NULL) {
                fprintf(stderr, "Syntax error: newline unexpected\n");
                exit_status = 127;
            } else {
                if ((fd = open(token, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0) {
                    fprintf(stderr, "Could not open file\n");
                    exit_status = 127;
                }

                final_output_fd = fd;
                modified = 1;
            }

        } else if(strcmp(token, ">>") == 0) {
            token = strtok(NULL, " ");

            if (token == NULL) {
                fprintf(stderr, "Syntax error: newline unexpected\n");
                exit_status = 127;
            } else {
                if ((fd = open(token, O_CREAT | O_WRONLY | O_APPEND, 0644)) < 0) {
                    fprintf(stderr, "Could not open file\n");
                    exit_status = 127;
                }

                final_output_fd = fd;
                modified =1;
            }

        } else if ((strcmp(token, "<")) == 0) {
            token = strtok(NULL, " ");
            if (token == NULL) {
                fprintf(stderr, "Syntax error: newline unexpected\n");
                exit_status = 127;
            } else {
                if ((fd = open(token, O_CREAT | O_RDONLY | O_APPEND, 0644)) < 0) {
                    fprintf(stderr, "Could not open file\n");
                    exit_status = 127;
                }

                final_input_fd = fd;
                modified = 1;
            }
            
        }
        
        if (modified) {
            if(dup2(final_output_fd, STDOUT_FILENO) < 0) {
                fprintf(stderr, "Could not dup file\n");
                exit_status = 127;
            }
            token = strtok(NULL, " ");
            continue;
        }

        tokenArray[*length] = token;
        (*length)++;
        token = strtok(NULL, " ");
    }

    return 0;
}


/**
 * 
 * */
char*    
deserialize_tokens(char *token) {
    
}

/**
 * Function for handling the builtin options for the shell.
 * */
bool    
handle_builtin(char **tokenArray, int len) {

    if(strcmp(tokenArray[0], "cd") == 0) {
        if(chdir(tokenArray[1]) != 0 ) {
            fprintf(stderr, "cd : %s\n", strerror(errno));
            exit_status = 127;
        }
    }
    else if (strcmp(tokenArray[0], "echo") == 0) {
        perform_echo(tokenArray, len);

    }
    else if (strcmp(tokenArray[0], "exit") == 0) {
        exit(0);
    }
    else {
        /* do nothing */
    }
    
    return true;
}

/**
 * Function for handling the echo builtin property
 * @param - array of tokens, length of the array
 * */
void    
perform_echo(char **tokenArray, int len) {
    char    *echo_str;

    if ((echo_str = malloc(ARG_MAX)) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
        exit_status = 127;
	}

    if(strcmp(tokenArray[1], "$$") == 0 ) {
        printf("%d\n", getpid());
    }

    else if(strcmp(tokenArray[1], "$?") == 0) {
        printf("%d\n", exit_status);
    }

    else {
        /* skipping the 1st element */
        for (int i = 1; i < len; i++) {
            strcat(echo_str, tokenArray[i]);
            strcat(echo_str, " ");
        }
        printf("%s\n", echo_str);
        bzero(echo_str, len);
    }

    (void) free(echo_str);
    
}

/**
 * Function to check if the given first argument or the token is a builtin.
 * */
bool
is_builtin(char **tokenArray, int len) {
    
    if((strcmp(*tokenArray, "cd") == 0) || 
        (strcmp(*tokenArray,  "echo") == 0) ||
        (strcmp(*tokenArray, "exit") == 0) )  {
            return  true;
    }

    return false;
}

int
fork_exec(char **tokenArray, int len) {
    pid_t   pid;
    int     w, status;

    if((pid=fork()) == -1) {
			fprintf(stderr, "shell: can't fork: %s\n",
					strerror(errno));
            exit_status = 127;
    }
    else if(pid == 0) {

        execvp(*tokenArray, tokenArray);
        fprintf(stderr, "-sish: %s: %s\n", *tokenArray,
					strerror(errno));
        exit(127);
    }
    else
    {
        w = waitpid(pid, &status, 0);
        exit_status = 127;
        if(WIFEXITED(status)) {
            // printf("Process terminated normally \n");
        }
        if(WIFSIGNALED(status)) {
            printf("abnormal termination, signal number = %d %s\n", WTERMSIG(status), strsignal(WTERMSIG(status)));
            printf("child process terminated abnormally \n");
        }
    }
    
}