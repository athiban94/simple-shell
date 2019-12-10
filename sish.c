#include "sish.h"

int 
main(int argc, char **argv)
{
    int     opt;
    int     trace;
    char    *c_string;
    char    buf[BUFFERSIZE];

    /* disable tracing at the begining */
    trace = 0; 

    /* setting the program name */
    (void) setprogname(argv[0]);

    if ((c_string = malloc(ARG_MAX)) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
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
        printf("input : %s\n", buf);
	}


    (void) free(c_string);
    return 0;
}


char* 
getinput(char *buffer, size_t buflen) {
    printf("$$ ");
    return fgets(buffer, buflen, stdin);
}