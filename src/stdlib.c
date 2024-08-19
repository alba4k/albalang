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
int combine(char **argv, const int mode) {
    if(argv[0] == NULL)
        return ERR_ARGC;
    if(argv[1] == NULL)
        return ERR_ARGC;

    Variable *var1 = find_var(&var_head, argv[0]);
    if(var1 == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var1->string != NULL)
        return ERR_WRONG_TYPE;
    
    Variable *var2 = eval(argv[1]);
    if(var2 == NULL)
        return ERR_OOM;
    if(var2->number == NULL)
        return ERR_GENERIC; // could be a variable not found, wrong variable type, or syntax error

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
            return ERR_GENERIC;
    }

    edit_var(var1, &value, NULL);

    return 0;
}

// add the contents of var2/end to var1
int fn_add(char **argv) {
    #ifdef DEBUG
    debug_log("Called add()");
    #endif // DEBUG
    
    return combine(argv, 0);
}

// concatenate the contents of var2/end with var1
int fn_concatenate(char **argv) {
    #ifdef DEBUG
    debug_log("Called concatenate()");
    #endif // DEBUG
    
    if(argv[0] == NULL)
        return ERR_ARGC;
    if(argv[1] == NULL)
        return ERR_ARGC;

    Variable *var1 = find_var(&var_head, argv[0]);
    if(var1 == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var1->number != NULL)
        return ERR_WRONG_TYPE;
    
    Variable *var2 = eval(argv[1]);
    if(var2 == NULL)
        return ERR_OOM;
    if(var2->string == NULL)
        return ERR_GENERIC; // could be a variable not found, wrong variable type, or syntax error

    char *string = var2->string;

    char *buf = malloc(strlen(var1->string) + strlen(var2->string) + 1);

    if(buf == NULL)
        return ERR_OOM;

    strcpy(buf, var1->string);
    strcat(buf, string);

    edit_var(var1, NULL, buf);

    del_var(var2);
    free(buf);

    return 0;
}

// delete a variable from memory
int fn_delete(char **argv) {
    #ifdef DEBUG
    debug_log("Called delete()");
    #endif // DEBUG

    if(argv[0] == NULL)
        return ERR_ARGC;

    Variable *var = find_var(&var_head, argv[0]);
    if(var != NULL) {
        del_var(var);

        return 0;
    }

    List *list = find_list(&list_head, argv[0]);
    if(list == NULL)
        return ERR_LIST_NOT_FOUND;

    del_list(list);

    return 0;
}

// divide the contents of var2/end by var1
int fn_divide(char **argv) {
    #ifdef DEBUG
    debug_log("Called divide()");
    #endif // DEBUG
    
    return combine(argv, 3);
}

// read a maximum of 0x4000 characters from stdin
int fn_input(char **argv) {
    #ifdef DEBUG
    debug_log("Called input()");
    #endif // DEBUG
    
    if(argv[0] == NULL)
        return ERR_ARGC;
        
    Variable *var = find_var(&var_head, argv[0]);
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
int fn_multiply(char **argv) {
    #ifdef DEBUG
    debug_log("Called multiply()");
    #endif // DEBUG
    
    return combine(argv, 2);
}

// cast str to num
int fn_num(char **argv) {
    #ifdef DEBUG
    debug_log("Called num()");
    #endif // DEBUG

    if(argv[0] == NULL)
        return ERR_ARGC;
        
    Variable *var = find_var(&var_head, argv[0]);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var->string == NULL)
        return ERR_WRONG_TYPE;
    
    double num = atof(var->string);

    edit_var(var, &num, NULL);

    return 0;
}

// raise var1 to the var2/end -th power
int fn_power(char **argv) {
    #ifdef DEBUG
    debug_log("Called power()");
    #endif // DEBUG
    
    return combine(argv, 4);
}

// print some text or a variable
int fn_print(char **argv) {
    #ifdef DEBUG
    debug_log("Called print()");
    #endif // DEBUG
    
    if(argv[0] == NULL)
        return ERR_ARGC;

    Variable *var = eval(argv[0]);
    if(var == NULL)
        return ERR_OOM;

    int newline = argv[1] ? atoi(argv[1]) : 1;
    int ret = 0;
    
    // newline can only be 0 or 1  
    if(newline != 0 && newline != 1) {
        del_var(var);
        return ERR_SYNTAX;
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
int fn_shell(char **argv) {
    #ifdef DEBUG
    debug_log("Called print()");
    #endif // DEBUG

    if(argv[0] == NULL)
        return ERR_ARGC;

    Variable *var = eval(argv[0]);
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
int fn_str(char **argv) {
    #ifdef DEBUG
    debug_log("Called num()");
    #endif // DEBUG

    if(argv[0] == NULL)
        return ERR_ARGC;
        
    Variable *var = find_var(&var_head, argv[0]);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var->number == NULL)
        return ERR_WRONG_TYPE;

    char str[256];
    snprintf(str, 256, "%f", *(var->number));

    edit_var(var, NULL, str);

    return 0;
}

// take the square root of the variable
int fn_sqrt(char **argv) {
    #ifdef DEBUG
    debug_log("Called sqrt()");
    #endif // DEBUG
    
    if(argv[0] == NULL)
        return ERR_ARGC;

    Variable *var = find_var(&var_head, argv[0]);
    if(var == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var->string != NULL)
        return ERR_WRONG_TYPE;
    
    double value = sqrt(*(var->number));
    edit_var(var, &value, NULL);

    return 0;
}

// add the contents of var2/end to var1
int fn_subtract(char **argv) {
    #ifdef DEBUG
    debug_log("Called subtract()");
    #endif // DEBUG
    
    return combine(argv, 1);
}

// print the type of var
int fn_type(char **argv) {
    #ifdef DEBUG
    debug_log("Called type()");
    #endif // DEBUG

    if(argv[0] == NULL)
        return ERR_ARGC;
    if(argv[1] == NULL)
        return ERR_ARGC;
        
    Variable *var1 = find_var(&var_head, argv[0]);
    Variable *var2 = find_var(&var_head, argv[1]);

    if(var1 == NULL || var2 == NULL)
        return ERR_VAR_NOT_FOUND;
    if(var1->number != NULL)
        edit_var(var2, NULL, "Number");
    else
        edit_var(var2, NULL, "String");

    return 0;
}

const struct Function functions[14] = {
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
