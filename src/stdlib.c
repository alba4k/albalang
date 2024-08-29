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
    if(var1->type != Number)
        return ERR_WRONG_TYPE;
    
    Variable *var2 = eval(argv[1]);
    if(var2 == NULL)
        return ERR_OOM;
    if(var2->type != Number)
        return ERR_GENERIC; // could be a variable not found, wrong variable type, or syntax error

    double value = var2->value.number;
    VariableValue result;
    
    del_var(var2);

    switch(mode) {
        case 0: // add
            result.number = var1->value.number + value;
            break;
        case 1: // subtract
            result.number = var1->value.number - value;
            break;
        case 2: // multiply
            result.number = var1->value.number * value;
            break;
        case 3: // divide
            result.number = var1->value.number / value;
            break;
        case 4: // power
            result.number = pow(var1->value.number, value);
            break;
        default:
            return ERR_GENERIC;
    }

    edit_var(var1, Number, result);

    return RET_OK;
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
    if(var1->type != String)
        return ERR_WRONG_TYPE;
    
    Variable *var2 = eval(argv[1]);
    if(var2 == NULL)
        return ERR_OOM;
    if(var2->type != String)
        return ERR_GENERIC; // could be a variable not found, wrong variable type, or syntax error

    VariableValue result;
    result.string = malloc(strlen(var1->value.string) + strlen(var2->value.string) + 1);

    if(result.string == NULL)
        return ERR_OOM;

    strcpy(result.string, var1->value.string);
    strcat(result.string, var2->value.string);

    edit_var(var1, String, result);

    del_var(var2);
    free(result.string);

    return RET_OK;
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

    return RET_OK;
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
    VariableValue input;
    input.string = malloc(MAX);

    #ifdef DEBUG
    printf("[\e[1m\e[36mINPUT\e[37m\e[0m] ");
    #endif // DEBUG

    fgets(input.string, MAX, stdin);

    char *end = strchr(input.string, '\n');
    if(end != NULL)
        *end = 0;

    edit_var(var, String, input);

    free(input.string);

    return RET_OK;
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
    if(var->type != String)
        return ERR_WRONG_TYPE;
    
    VariableValue result;
    result.number = atof(var->value.string);

    edit_var(var, Number, result);

    return RET_OK;
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

    if(var->type == Number)
        printf("%f%s", var->value.number, newline ? "\n" : "");
    else if(var->type == String)
        printf("%s%s", var->value.string, newline ? "\n" : "");
    else {  // shouldn't happen but still
        del_var(var);
        return ERR_GENERIC;
    }

    del_var(var);

    return RET_OK;
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
        
    if(var->type == Number) {
        del_var(var);
        return ERR_WRONG_TYPE;
    }
    if(var->type != String) {
        del_var(var);
        return ERR_GENERIC;
    }
    
    if(fork() != 0)
        execlp("/bin/sh", "sh", "-c", var->value.string, NULL);
    
    wait(0);

    return RET_OK;
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
    if(var->type != Number)
        return ERR_WRONG_TYPE;

    VariableValue result;
    result.string = malloc(256);
    snprintf(result.string, 256, "%f", var->value.number);

    edit_var(var, String, result);

    free(result.string);

    return RET_OK;
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
    if(var->type != Number)
        return ERR_WRONG_TYPE;
    
    VariableValue result;
    result.number = sqrt(var->value.number);
    edit_var(var, Number, result);

    return RET_OK;
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

    VariableValue result;

    if(var1->type == Number)
        result.string = "Number";
    else if(var1->type == String)
        result.string = "String";
    else
        result.string = "Unassigned";

    edit_var(var2, String, result);

    return RET_OK;
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
