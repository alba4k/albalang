#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"

int fn_print(char *str) {
    char *start;
    
    if((start = strchr(str, '"'))) {
        ++start;
        char *end;

        if(!(end = strchr(start, '"')))
            return -1;

        *end = 0;

        int newline = 1;

        // newline ?
        char *second_arg = strchr(end+1, ',');
        if(second_arg) {
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

    return -1;
}

const struct function functions[1] = {
    {"print", fn_print},
};
