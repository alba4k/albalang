#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"
#include "variables.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

// print some text or a variable
int fn_print(char *str) {
    #ifdef DEBUG
    debug_log("Called print with args `%s`", str);
    #endif // DEBUG
    
    char *start = skip_whites(str);
    
    if(start[0] == '"') {
        ++start;
        char *end = strchr(start, '"');

        if(!end)
            return -1;
            
        *end = 0;

        int newline = 1;

        // newline ?
        char *second_arg = skip_whites(end+1);
        if(second_arg != NULL)
            if(second_arg[0] == ',') {
                ++second_arg;
                newline = atoi(second_arg);
            }

        // newline can only be 0 or 1  
        if(newline != 0 && newline != 1) {
            *end = '"';
            return -1;
        }

        printf("%s%s", start, newline ? "\n" : "");

        *end = '"';

        return 0;
    }
    else if(start[0] == '$') {
        if(start[1] != '{')
            return -1;

        char *end = strchr(start, '}');
        if(!end)
            return -1;

        start += 2;
        *end = 0;
        
        int ret = -1;

        Variable *var = find_var(&var_head, start);
        if(var == NULL) 
            goto error;
        
        int newline = 1;

        // newline ?
        char *second_arg = skip_whites(end+1);
        if(second_arg)
            if(second_arg[0] == ',') {
                ++second_arg;
                newline = atoi(second_arg);
            }

        // newline can only be 0 or 1  
        if(newline != 0 && newline != 1) {
            goto error;
        }

        if(var->number) {
            printf("%f%s", *(var->number), newline ? "\n" : "");
            ret = 0;
        }
        else if(var->string) {
            printf("%s%s", var->string, newline ? "\n" : "");
            ret = 0;
        }

        error:

        *end = '}';

        return ret;

    }

    return -1;
}

// delete a variable from memory
int fn_delete(char *name) {
    #ifdef DEBUG
    debug_log("Called delete with args `%s`", name);
    #endif // DEBUG

    name = skip_whites(name);
    if(!name)
        return -1;

    char *end = skip_full(name);
    if(!end)
        end = name + strlen(name);
    char cache = *end;
    *end = 0;

    int ret = del_var(find_var(&var_head, name));

    *end = cache;

    return ret;
}

const struct function functions[2] = {
    {"print", fn_print},
    {"delete", fn_delete},
};
