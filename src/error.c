#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "variables.h"

void error(const char *message, const char *extra, const int code, void *memory) {
    fflush(stdout);

    fprintf(stderr, "[\e[1m\e[31mERROR\e[37m\e[0m] %s\n", message);
    if(extra)
        fprintf(stderr, "        \e[31m>>>\e[0m %s \e[31m<<<\e[0m\n", extra);

    fputs("       \e[31m\e[1m<!>\e[0m ", stderr);
    switch(code) {
        case ERR_BAD_USAGE:
            fputs("Too little or too many arguments\n", stderr);
            break;
        case ERR_NO_SUCH_FILE:
            fputs("Please use an existing file\n", stderr);
            break;
        case ERR_FILE_UNREADABLE:
            fputs("Access denied\n", stderr);
            break;
        case ERR_GENERIC:
            fputs("Generic error\n", stderr);
            break;
        case ERR_SYNTAX:
            fputs("Bad syntax\n", stderr);
            break;
        case ERR_VAR_NOT_FOUND:
            fputs("Variable not found\n", stderr);
            break;
        case ERR_WRONG_TYPE:
            fputs("Wrong variable type\n", stderr);
            break;
        default:
            fputs("Unknown error\n", stderr);
    }

    // clear variable linked list
    while(var_head.next != NULL)
        del_var(var_head.next);

    free(memory);
    exit(code);
}