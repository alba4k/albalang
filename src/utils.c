#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "stdlib.h"

void error(const char *message, const char *extra, const int code, void *memory) {
    fprintf(stderr, "[\e[1m\e[31mERROR\e[37m\e[0m]: %s\n", message);
    if(extra)
        fprintf(stderr, "         \e[31m>>>\e[0m %s \e[31m<<<\e[0m\n", extra);

    free(memory);
    exit(code);
}

int eval(const char *code) {
    char *ptr;
    int ret = 0;

   // functions belonging to stdlib
    for(int i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        if((ptr = strstr(code, functions[i].name))) {
            ptr += strlen(functions[i].name);

            char *args = strchr(ptr, '(');
            if(!args)
                return -1;
            ++args;
            
            char *argend = strchr(args, ')');
            if(!argend)
                return -1;
            
            *argend = 0;
            ret = functions[i].func(args);
            *argend = ')';
        }
    }

    return ret;
}
