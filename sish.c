#include "sish.h"

int 
main(int argc, char **argv) {
    int     opt;
   
    char    *c_string;
    char    buf[BUFFERSIZE];
    char    **tokenArray;
    int     token_length = 0;
    char    *is_pipe;
    char    **p_tokArray;
    int     p_toklen = 0;
    int     pipe_array[2];
    pid_t   pid;
    int     w, status;
    int     pipe_fd;

    
    exit_status = 0;

    /* disable tracing at the begining */
    trace = 0; 

    /* initializing beinging STD_IN */
    pipe_fd = STDIN_FILENO;

    /* setting the program name */
    (void) setprogname(argv[0]);
     if( (original_fd_out = dup(STDOUT_FILENO)) < 0) {
            fprintf(stderr, "Could not dup file\n");
            exit_status = 127;
        }

        if( (original_fd_in = dup(STDIN_FILENO)) < 0) {
            fprintf(stderr, "Could not dup file\n");
            exit_status = 127;
        }

    if ((c_string = malloc(ARG_MAX)) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
		return 1;
	}

    if ((tokenArray = malloc(ARG_MAX * sizeof(char*))) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
		return 1;
	}

    if ((p_tokArray = malloc(ARG_MAX * sizeof(char*))) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
		return 1;
	}

    if (signal(SIGINT, sig_int) == SIG_ERR) {
		fprintf(stderr, "signal error: %s\n", strerror(errno));
		return 1;
	}

    while ((opt = getopt(argc, argv, "c:x")) != -1) {
        switch (opt) {
            case 'c':
                c_string = optarg;
                break;
            case 'x':
                 trace = 1;
                break;
            case '?':
                fprintf(stderr, "sish[−x][−c command]\n");
                return 1;
            default:
                break;
        }
    }

    while (getinput(buf, sizeof(buf))) {
		buf[strlen(buf) - 1] = '\0';

        is_pipe = strchr(buf, '|');
        if(is_pipe != NULL) {

            if(tokenize_pipe_cmds(buf, tokenArray, &token_length)  == 0)  {


                for (int i = 0; i < token_length; i++)
                {
                    if(trace) {
                        printf("+ %s\n", tokenArray[i]);
                    }
                    if(pipe(pipe_array) < 0) {

                        fprintf(stderr, "sish: pipe error: %s\n",strerror(errno));
                        exit_status = 127;

                    } else {

                        if((pid=fork()) == -1) {
                                fprintf(stderr, "shell: can't fork: %s\n",strerror(errno));
                                exit_status = 127;
                        }
                        else if(pid == 0) {

                            if(i < token_length - 1) {

                                if(dup2(pipe_array[1], STDOUT_FILENO) < 0) {
                                    fprintf(stderr, "Could not dup file\n");
                                    exit_status = 127;
                                }

                            }

                            if(dup2(pipe_fd, STDIN_FILENO) < 0) {
                                fprintf(stderr, "Could not dup file\n");
                                exit_status = 127;
                            }
                            
                            execute_buf(tokenArray[i], p_tokArray, &p_toklen);
                            exit(127);

                        }
                        else {
                            w = waitpid(pid, &status, 0);
                            if(status != 0) {
                                exit_status = 127;
                            } else {
                                exit_status = status;
                            }
                            (void) close(pipe_array[1]);
                            pipe_fd = pipe_array[0];
                        }
                    } 
                }
                token_length = 0;
                bzero(tokenArray, ARG_MAX * sizeof(char*));
            }

        } else  {
            execute_buf(buf, tokenArray, &token_length);
        }
        

	}

    (void) free(c_string);
    return 0;
}


void
execute_buf(char *buf, char **tokenArray, int *token_length) {

        if(tokenize(buf, tokenArray, token_length)  == 0 ) {
            if(*token_length) {
                if(trace == 1) {
                    printf("+ %s\n", *tokenArray);
                }
                if(is_builtin(tokenArray, *token_length)) {
                    handle_builtin(tokenArray, *token_length);
                }
                else {
                    fork_exec(tokenArray, *token_length);
                }
            }
        }

        *token_length = 0;
        bzero(tokenArray, ARG_MAX * sizeof(char*));

        if(dup2(original_fd_out, STDOUT_FILENO) < 0) {
            fprintf(stderr, "Could not dup file\n");
            exit_status = 127;
        }

        if(dup2(original_fd_in, STDIN_FILENO) < 0) {
            fprintf(stderr, "Could not dup file\n");
            exit_status = 127;
        }
}


void
sig_int(int signo) {
    exit_status = 127;
}