#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "core.h"
#include "error.h"
#include "stdlib.h"
#include "utils.h"
#include "datastructures/variables.h"
#include "datastructures/lists.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

// this function is called by most binary operators
int combine(char *str, const int mode) {
    char *var1_name = skip_whites(str);
    char *end = strchr(var1_name, ',');

    if(end == NULL)
        return ERR_SYNTAX;

    *end = 0;
    char *cache = end;

    Variable *var1 = find_var(&var_head, var1_name);

    if(var1 == NULL) {
        *(cache) = ',';
        
        return ERR_VAR_NOT_FOUND;
    }
    if(var1->string != NULL) {
        *(cache) = ',';
        return ERR_WRONG_TYPE;
    }
    
    end = skip_whites(end+1);
    if(end == NULL) {
        *(cache) = ',';
        return ERR_SYNTAX;
    }
    
    Variable *var2 = eval(end);
    if(var2 == NULL)
        return ERR_OOM;

    if(var2->number == NULL) {
        *(cache) = ',';
        return ERR_GENERIC; // could be a variable not found, wrong variable type, or syntax error
    }

    double value = *var2->number;
    
    del_var(var2);

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
            *(cache) = ',';
            return ERR_SYNTAX;
    }

    edit_var(var1, &value, NULL);

    *cache = ',';
    return 0;
}

// add the contents of var2/end to var1
int fn_add(char *str) {
    #ifdef DEBUG
    debug_log("Called add() with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 0);
}

// concatenate the contents of var2/end with var1
int fn_concatenate(char *str) {
    #ifdef DEBUG
    debug_log("Called concatenate() with args `%s`", str);
    #endif // DEBUG
    
    char *var1_name = skip_whites(str);
    char *end = strchr(var1_name, ',');

    if(end == NULL)
        return ERR_SYNTAX;

    *end = 0;
    char *cache = end;

    Variable *var1 = find_var(&var_head, var1_name);

    if(var1 == NULL) {
        *(cache) = ',';
        return ERR_VAR_NOT_FOUND;
    }
    if(var1->number != NULL) {
        *(cache) = ',';
        return ERR_WRONG_TYPE;
    }
    
    end = skip_whites(end+1);
    if(end == NULL) 
        return ERR_SYNTAX;

    Variable *var2 = eval(end);
    if(var2 == NULL)
        return ERR_OOM;

    if(var2->string == NULL) {
        *(cache) = ',';
        return ERR_GENERIC; // could be a variable not found, wrong variable type, or syntax error
    }

    char *string = var2->string;

    char *buf = malloc(strlen(var1->string) + strlen(var2->string) + 1);

    if(buf == NULL)
        return ERR_OOM;

    strcpy(buf, var1->string);
    strcat(buf, string);

    edit_var(var1, NULL, buf);

    del_var(var2);
    free(buf);

    *cache = ',';
    return 0;
}

// delete a variable from memory
int fn_delete(char *name) {
    #ifdef DEBUG
    debug_log("Called delete() with args `%s`", name);
    #endif // DEBUG

    name = skip_whites(name);
    if(name == NULL)
        return ERR_SYNTAX;

    Variable *var = find_var(&var_head, name);
    if(var != NULL) {
        del_var(var);

        return 0;
    }

    List *list = find_list(&list_head, name);
    if(list == NULL)
        return ERR_LIST_NOT_FOUND;

    del_list(list);

    return 0;
}

// divide the contents of var2/end by var1
int fn_divide(char *str) {
    #ifdef DEBUG
    debug_log("Called divide() with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 3);
}

// read a maximum of 0x4000 characters from stdin
int fn_input(char *name) {
    #ifdef DEBUG
    debug_log("Called input() with args `%s`", name);
    #endif // DEBUG
    
    Variable *var = find_var(&var_head, name);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;

    const int MAX = 0x4000;
    char buf[MAX];

    #ifdef DEBUG
    printf("[\e[1m\e[36mINPUT\e[37m\e[0m] ");
    #endif // DEBUG

    fgets(buf, 0x4000, stdin);

    char *end = strchr(buf, '\n');
    if(end != NULL)
        *end = 0;

    edit_var(var, NULL, buf);

    return 0;
}

// add the contents of var2/end to var1
int fn_multiply(char *str) {
    #ifdef DEBUG
    debug_log("Called multiply() with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 2);
}

// cast str to num
int fn_num(char *name) {
    #ifdef DEBUG
    debug_log("Called num() with args `%s`", name);
    #endif // DEBUG

    name = skip_whites(name);
    if(name == NULL)
        return ERR_SYNTAX;

    char *end = skip_full(name);
    if(end == NULL)
        end = name + strlen(name);
    *end = 0;

    Variable *var = find_var(&var_head, name);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var->string == NULL)
        return ERR_WRONG_TYPE;
    
    double num = atof(var->string);

    edit_var(var, &num, NULL);

    return 0;
}

// raise var1 to the var2/end -th power
int fn_power(char *str) {
    #ifdef DEBUG
    debug_log("Called power() with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 4);
}

// print some text or a variable
int fn_print(char *str) {
    #ifdef DEBUG
    debug_log("Called print() with args `%s`", str);
    #endif // DEBUG
    
    str = skip_whites(str);

    // using numbers might give weirds results in case of something like `print(4.3, 0);` (I therefore return ERR_GENERIC)
    Variable *var = eval(str);
    if(var == NULL)
        return ERR_OOM;

    int newline = 1;
    int ret = 0;

    char *end;
    if(var->name != NULL)
        end = str + strlen(var->name) + 3;
    else if(var->string != NULL)
        end = str + strlen(var->string) + 2;
    else {
        del_var(var);
        return ERR_GENERIC;
    }
    
    // newline ?
    char *second_arg = skip_whites(end);
    if(second_arg != NULL)
        if(second_arg[0] == ',') {
            ++second_arg;
            newline = atoi(second_arg);
        }

    // newline can only be 0 or 1  
    if(newline != 0 && newline != 1) {
        del_var(var);
        return ERR_GENERIC;
    }

    #ifdef DEBUG
    if(newline == 0) {
        debug_log("Enforcing newline = 1 because of logging");
        newline = 1;
    }
    printf("[\e[1m\e[32mPRINT\e[37m\e[0m] ");
    #endif // DEBUG

    if(var->number)
        printf("%f%s", *(var->number), newline ? "\n" : "");
    else if(var->string)
        printf("%s%s", var->string, newline ? "\n" : "");

    del_var(var);

    return ret;
}

// execute cmd in a shell
int fn_shell(char *cmd) {
    #ifdef DEBUG
    debug_log("Called print() with args `%s`", cmd);
    #endif // DEBUG

    Variable *var = eval(cmd);
    if(var == NULL)
        return ERR_OOM;
        
    if(var->number != NULL) {
        del_var(var);
        return ERR_WRONG_TYPE;
    }
    if(var->string == NULL) {
        del_var(var);
        return ERR_GENERIC;
    }
    
    if(fork() != 0)
        execlp("/bin/sh", "sh", "-c", var->string, NULL);
    
    wait(0);

    return 0;
}

// cast num to str
int fn_str(char *name) {
    #ifdef DEBUG
    debug_log("Called num() with args `%s`", name);
    #endif // DEBUG

    name = skip_whites(name);
    if(name == NULL)
        return ERR_SYNTAX;

    char *end = skip_full(name);
    if(end == NULL)
        end = name + strlen(name);
    *end = 0;

    Variable *var = find_var(&var_head, name);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var->number == NULL)
        return ERR_WRONG_TYPE;

    char str[512];
    sprintf(str, "%f", *(var->number));

    edit_var(var, NULL, str);

    return 0;
}
// take the square root of the variable
int fn_sqrt(char *str) {
    #ifdef DEBUG
    debug_log("Called sqrt() with args `%s`", str);
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
        return ERR_VAR_NOT_FOUND;
    if(var->string != NULL)
        return ERR_WRONG_TYPE;
    
    double value = sqrt(*(var->number));
    edit_var(var, &value, NULL);

    return 0;
}

// add the contents of var2/end to var1
int fn_subtract(char *str) {
    #ifdef DEBUG
    debug_log("Called subtract() with args `%s`", str);
    #endif // DEBUG
    
    return combine(str, 1);
}

// print the type of var
int fn_type(char *name) {
    #ifdef DEBUG
    debug_log("Called type() with args `%s`", name);
    #endif // DEBUG

    name = skip_whites(name);
    if(name == NULL)
        return ERR_SYNTAX;

    char *end = skip_full(name);
    if(end == NULL)
        end = name + strlen(name);
    *end = 0;

    Variable *var = find_var(&var_head, name);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var->string != NULL) {
        #ifdef DEBUG
        printf("[\e[1m\e[32mPRINT\e[37m\e[0m] ");
        #endif // DEBUG
        printf("string\n");
    }
    else {
        #ifdef DEBUG
        printf("[\e[1m\e[32mPRINT\e[37m\e[0m] ");
        #endif // DEBUG
        printf("number\n");
    }

    return 0;
}

const struct function functions[14] = {
    {"add", fn_add},
    {"concatenate", fn_concatenate},
    {"delete", fn_delete},
    {"divide", fn_divide},
    {"input", fn_input},
    {"multiply", fn_multiply},
    {"num", fn_num},
    {"power", fn_power},
    {"print", fn_print},
    {"shell", fn_shell},
    {"str", fn_str},
    {"sqrt", fn_sqrt},
    {"subtract", fn_subtract},
    {"type", fn_type},
};
