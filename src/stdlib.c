#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"
#include "variables.h"

// print some text or a variable
int fn_print(char *str) {
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
        
        Number *nvar = find_num(&num_head, start);
        String *svar;
        if(!nvar) {
            svar = find_str(&str_head, start);
            if(!svar) {
                *end = '}';
                return -1;
            }
        }
        
        int newline = 1;

        // newline ?
        char *second_arg = skip_whites(end);
        if(second_arg)
            if(second_arg[0] == ',') {
                ++second_arg;
                newline = atoi(second_arg);
            }

        // newline can only be 0 or 1  
        if(newline != 0 && newline != 1) {
            *end = '}';
            return -1;
        }

        if(nvar)
            printf("%f%s", nvar->value, newline ? "\n" : "");
        else
            printf("%s%s", svar->value, newline ? "\n" : "");

        *end = '}';

        return 0;

    }

    return -1;
}

// delete a variable from memory
int fn_delete(char *name) {
    name = skip_whites(name);
    if(!name)
        return -1;

    char *end = skip_full(name);
    if(!end)
        end = name + strlen(name);
    char cache = *end;
    *end = 0;

    Number *nvar = NULL;
    String *svar = NULL;
    for(Number *current = num_head.next; current; current = current->next)
        if(!strcmp(name, current->name)) {
            nvar = current;
            break;
        }
    if(!nvar) {
        for(String *current = str_head.next; current; current = current->next)
            if(!strcmp(name, current->name)) {
                svar = current;
                break;
            }
    }
    if(!svar) {
        return -1;
    }
    
    if(nvar)
        del_num(nvar);
    else
        del_str(svar);

    return 0;
}

const struct function functions[2] = {
    {"print", fn_print},
    {"delete", fn_delete},
};
