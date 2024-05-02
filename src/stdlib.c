#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"
#include "variables.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

// this function is called by most binary operators
int combine(char *str, const int mode) {
    char *var1_name = skip_whites(str);
    char *end = strchr(var1_name, ',');

    if(end == NULL)
        return -1;

    *end = 0;
    char *cache = end;

    Variable *var1 = find_var(&var_head, var1_name);

    if(var1 == NULL)
        goto error;
    if(var1->string != NULL)
        goto error;
    
    end = skip_whites(end+1);
    if(end == NULL) 
        return -1;

    double value = 0;

    if(end[0] == '$') {
        Variable *var2 = access_var(end);

        if(var2 == NULL)
            goto error;
        if(var2->string != NULL)
            goto error;
        
        value = *(var2->number);
    }
    else
        value = atof(end);

    switch(mode) {
    case 0: // add
        value = *(var1->number) + value;
        break;
    case 1: // subtract
        value = *(var1->number) - value;
        break;
    case 2: // multiply
        value = *(var1->number) * value;
        break;
    case 3: // divide
        value = *(var1->number) / value;
        break;
    case 4: // power
        value = pow(*(var1->number), value);
        break;
    default:
        goto error;
    }

    edit_var(var1, &value, NULL);

    *cache = ',';
    return 0;

    error:

    *(cache) = ',';
    return -1;
}

// add the contents of var2/end to var1
int fn_add(char *str) {
    #ifdef DEBUG
    debug_log("Called add with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 0);
}

// concatenate the contents of var2/end with var1
int fn_concatenate(char *str) {
    #ifdef DEBUG
    debug_log("Called concatenate with args `%s`", str);
    #endif // DEBUG
    
    char *var1_name = skip_whites(str);
    char *end = strchr(var1_name, ',');

    if(end == NULL)
        return -1;

    *end = 0;
    char *cache = end;

    Variable *var1 = find_var(&var_head, var1_name);

    if(var1 == NULL)
        goto error;
    if(var1->number != NULL)
        goto error;
    
    end = skip_whites(end+1);
    if(end == NULL) 
        return -1;

    char *string = 0;

    if(end[0] == '$') {
        Variable *var2 = access_var(end);

        if(var2 == NULL)
            goto error;
        if(var2->number != NULL)
            goto error;
        
        string = var2->string;
    }
    else if(end[0] == '"') {
        string = end+1;

        end = strchr(string, '"');
        if(end == NULL)
            goto error;
    }

    char *buf = malloc(strlen(var1->string) + (end - string) + 1);

    strcpy(buf, var1->string);
    strncat(buf, string, end-string);

    edit_var(var1, NULL, buf);

    free(buf);

    *cache = ',';
    return 0;

    error:

    *(cache) = ',';
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

// divide the contents of var2/end by var1
int fn_divide(char *str) {
    #ifdef DEBUG
    debug_log("Called divide with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 3);
}

// add the contents of var2/end to var1
int fn_multiply(char *str) {
    #ifdef DEBUG
    debug_log("Called multiply with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 2);
}

// raise var1 to the var2/end -th power
int fn_power(char *str) {
    #ifdef DEBUG
    debug_log("Called power with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 4);
}

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
        Variable *var = access_var(start);
        if(var == NULL) 
            return -1;
        
        int newline = 1;

        // newline ?
        char *second_arg = skip_whites(start + 3 + strlen(var->name));
        if(second_arg)
            if(second_arg[0] == ',') {
                ++second_arg;
                newline = atoi(second_arg);
            }

        // newline can only be 0 or 1  
        if(newline != 0 && newline != 1)
            return -1;

        if(var->number)
            printf("%f%s", *(var->number), newline ? "\n" : "");
        else if(var->string)
            printf("%s%s", var->string, newline ? "\n" : "");

        return 0;
    }

    return -1;
}

// take the square root of the variable
int fn_sqrt(char *str) {
    #ifdef DEBUG
    debug_log("Called sqrt with args `%s`", str);
    #endif // DEBUG
    
    char *name = skip_whites(str);
    char *end = skip_full(name);
    char cache = 0;

    if(end != NULL) {
        cache = *end;
        *end = 0;
    }

    Variable *var = find_var(&var_head, name);

    if(end != NULL)
        *end = cache;

    if(var == NULL)
        return -1;
    if(var->string != NULL)
        return -1;
    
    double value = sqrt(*(var->number));
    edit_var(var, &value, NULL);

    return 0;
}

// add the contents of var2/end to var1
int fn_subtract(char *str) {
    #ifdef DEBUG
    debug_log("Called subtract with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 1);
}

const struct function functions[9] = {
    {"add", fn_add},
    {"concatenate", fn_concatenate},
    {"delete", fn_delete},
    {"divide", fn_divide},
    {"multiply", fn_multiply},
    {"power", fn_power},
    {"print", fn_print},
    {"sqrt", fn_sqrt},
    {"subtract", fn_subtract},
};
