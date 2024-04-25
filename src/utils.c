#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"
#include "variables.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

void error(const char *message, const char *extra, const int code, void *memory) {
    fflush(stdout);

    fprintf(stderr, "[\e[1m\e[31mERROR\e[37m\e[0m]: %s\n", message);
    if(extra)
        fprintf(stderr, "         \e[31m>>>\e[0m %s \e[31m<<<\e[0m\n", extra);

    // clear variable linked list
    while(var_head.next != NULL)
        del_var(var_head.next);

    free(memory);
    exit(code);
}

void uncomment(char *text) {
    /*
    #ifdef DEBUG
    debug_log("Removing comments...");
    #endif // DEBUG
    */

    char *ptr = text;
    while((ptr = strchr(ptr, '#'))) {
        if(ptr > text) {
            if(ptr[-1] == '\\') {
                memmove(ptr -1 , ptr, strlen(ptr)+1);
                ++ptr;

                continue;
            }
        }

        char *ptr2 = strchr(ptr, '\n');
        if(ptr2)
            memmove(ptr, ptr2 + 1, strlen(ptr2)+1);
        else
            *ptr = 0;

        ++ptr;
    }
}

char *skip_whites(char *ptr) {
    if(!ptr)
        return NULL;
    if(!ptr[0])
        return NULL;

    while(*ptr) {
        if(*ptr != ' ' && *ptr != '\n')
            return ptr;
        ++ptr;
    }

    return NULL;
}

char *skip_full(char *ptr) {
    if(!ptr)
        return NULL;
    if(!ptr[0])
        return NULL;

    while(*ptr) {
        if(*ptr == ' ' || *ptr == '\n')
            return ptr;
        ++ptr;
    }

    return NULL;
}

int eval(char *code) {
    #ifdef DEBUG
    debug_log("Running a new line");
    #endif // DEBUG

    char *ptr;
    int ret = 1;

    code = skip_whites(code);

    // functions belonging to stdlib
    for(size_t i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        if((ptr = strstr(code, functions[i].name))) {
            ptr += strlen(functions[i].name);

            char *args = skip_whites(ptr);
            if(args[0] != '(')
                return -1;
            ++args;
            
            char *argend = strchr(args, ')');
            if(!argend)
                return -1;
                
            *argend = 0;
            
            ret = functions[i].func(args);
            *argend = ')';
            return ret;
        }
    }

    // we have something like `  var variable = 80.9  `
    if(!strncmp(code, "var", 3)) {
        // `variable = 80.9`
        ptr = skip_whites(code+3);
        if(!ptr)
            return -1;

        ptr = skip_whites(ptr);
        if(!ptr)
            return -1;
            
        // `= 80.9`
        char *ptr2 = skip_full(ptr);
        if(!ptr2)
            return -1;
        char cache = *ptr2;
        *ptr2 = 0;
        char *name = ptr;
        ptr = skip_whites(ptr2+1);

        // `80.9`
        if(ptr[0] != '=') 
            goto error;
        ++ptr;
        ptr2 = skip_whites(ptr);
        if(ptr2[0] == '"') {
            // create a str variable with [name] and [ptr2]
            ++ptr2;

            ptr = strchr(ptr2, '"');
            if(!ptr)
                goto error;
            *ptr = 0;

            add_var(&var_head, name, NULL, ptr2);

            *ptr = '"';

            ret = 0;
        }
        else {
            // create a num variable with [name] and [value]
            double value = atof(ptr2);
            add_var(&var_head, name, &value, NULL);

            ret = 0;
        }

        error:

        *ptr2 = cache;

        return ret;
    }

    return ret;
}
