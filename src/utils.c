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

int eval(const char *code) {
    #ifdef DEBUG
    debug_log("Running a new line");
    #endif // DEBUG

    char *ptr;
    int ret = 0;

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

    // we have something like `  var num variable = 80.9  `
    if((ptr = strstr(code, "var"))) {
        // `num variable = 80.9`
        char *ptr2 = skip_whites(ptr+3);
        if(!ptr2)
            return -1;
        
        if(!strncmp(ptr2, "num", 3)) {
            // ` variable = 80.9`
            ptr2 += 3;

            // `variable = 80.9`
            ptr2 = skip_whites(ptr2);
            if(!ptr2)
                return -1;

            // `= 80.9`
            char name[32];
            char *ptr3 = skip_full(ptr2);
            if(!ptr3)
                return -1;
            char cache = *ptr3;
            *ptr3 = 0;
            strncpy(name, ptr2, 32);
            *ptr3 = cache;
            ptr2 = skip_whites(ptr3);

            // `80.9`
            if(ptr2[0] != '=')
                return -1;
            ++ptr2;
            ptr3 = skip_whites(ptr2);

            // create a variable with [name] and [value]
            double value = atof(ptr3);
            add_var(&var_head, name, &value, NULL);
        }
        if(!strncmp(ptr2, "str", 3)) {
            // ` variable = "80.9"`
            ptr2 += 3;

            // `variable = "80.9"`
            ptr2 = skip_whites(ptr2);
            if(!ptr2)
                return -1;

            // `= "80.9"`
            char name[32];
            char *ptr3 = skip_full(ptr2);
            if(!ptr3)
                return -1;
            char cache = *ptr3;
            *ptr3 = 0;
            strncpy(name, ptr2, 32);
            *ptr3 = cache;
            ptr2 = skip_whites(ptr3);

            // `"80.9"`
            if(ptr2[0] != '=')
                return -1;
            ++ptr2;
            ptr3 = skip_whites(ptr2);

            // create a variable with [name] and [value]
            if(*ptr3 != '"')
                return -1;
            ++ptr3;

            ptr2 = strchr(ptr3, '"');
            if(!ptr2)
                return -1;
            *ptr2 = 0;
            
            add_var(&var_head, name, NULL, ptr3);

            *ptr2 = '"';
        }
    }

    return ret;
}
