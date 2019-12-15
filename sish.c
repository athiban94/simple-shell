#include "sish.h"

int 
main(int argc, char **argv)
{
    int     opt;
    int     trace;
    char    *c_string;
    char    buf[BUFFERSIZE];
    char    **tokenArray;
    int     token_length = 0;
    
    exit_status = 0;

    /* disable tracing at the begining */
    trace = 0; 

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

    if (signal(SIGINT, sig_int) == SIG_ERR) {
		fprintf(stderr, "signal error: %s\n", strerror(errno));
		return 1;
	}

    while ((opt = getopt(argc, argv, "c:x:")) != -1) {
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

        if(tokenize(buf, tokenArray, &token_length)  == 0 ) {
            if(token_length) {
                if(is_builtin(tokenArray, token_length)) {
                    handle_builtin(tokenArray, token_length);
                }
                else {
                    fork_exec(tokenArray, token_length);
                }
            }
        }

        token_length = 0;
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

    (void) free(c_string);
    return 0;
}


void
sig_int(int signo) {
	printf("\nCaught SIGINT!\n");
    exit_status = 127;
}