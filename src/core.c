#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "error.h"
#include "stdlib.h"
#include "utils.h"
#include "variables.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

Variable *eval(char *expression) {
    #ifdef DEBUG
    debug_log("Trying to evaluate `%s`", expression);
    #endif // DEBUG

    /* This can currently parse:
     * ${var}
     * "text"
     * 4.5
     */

    Variable *result = malloc(sizeof(Variable));
    memset(result, 0, sizeof(Variable));
    
    // this will eventually be a while loop I think

    expression = skip_whites(expression);

    if(expression == NULL)
        return result;

    if(expression[0] == '$') {
        Variable *var = access_var(expression);
        if(var == NULL)
            return result;

        result->name = realloc(result->name, strlen(var->name) + 1);
        strcpy(result->name, var->name);

        if(var->number != NULL) {
            result->number = realloc(result->number, sizeof(double));
            *result->number = *var->number;
        }
        else if(var->string != NULL) {
            result->string = realloc(result->string, strlen(var->string)+1);
            strcpy(result->string, var->string);
        }
    }
    else if(expression[0] == '"') {
        char *end = strchr(expression+1, '"');
        if(end == NULL)
            return result;
        
        *end = 0;
        result->string = realloc(result->string, strlen(expression+1)+1);
        strcpy(result->string, expression+1);
        *end = '"';
    }
    else {
        result->number = realloc(result->number, sizeof(double));
        *result->number = atof(expression);
    }

    return result;
}

int run_line(char *code) {
    #ifdef DEBUG
    debug_log("Running a new line");
    #endif // DEBUG

    char *ptr;

    code = skip_whites(code);

    // we have something like `  var   variable = 80.9  `
    if(strncmp(code, "var ", 4) == 0) {
        ptr = skip_whites(code+4);
        if(ptr == NULL)
            return ERR_SYNTAX;

        // `variable = 80.9`

        char *name = ptr;

        char *end = strchr(ptr, '=');
        if(end == NULL)
            return ERR_SYNTAX;
        char cache = 0;

        *end = 0;

        char *ptr2 = skip_full(ptr);
        if(ptr2 != NULL) {
            *end = '=';
            cache = *ptr2;
            *ptr2 = 0;
        }

        // `= 80.9`

        Variable *value = eval(end+1);
        
        // read ` 80.9`

        // they are both NULL
        if(value->number == NULL && value->string == NULL) {
            if(ptr2 != NULL)
                *ptr2 = cache;
            return ERR_SYNTAX;
        }

        Variable *var = find_var(&var_head, name);
        if(var == NULL)
            add_var(&var_head, name, value->number, value->string);
        else
            edit_var(var, value->number, value->string);

        del_var(value);

        if(ptr2 != NULL)
            *ptr2 = cache;

        return 0;
    }

    // functions belonging to stdlib
    for(size_t i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        if((ptr = strstr(code, functions[i].name))) {
            ptr += strlen(functions[i].name);
            
            char *args = skip_whites(ptr);
            if(args[0] != '(')
                return ERR_SYNTAX;
            ++args;
            
            char *argend = strchr(args, ')');
            if(!argend)
                return ERR_SYNTAX;
                
            *argend = 0;
            
            int ret = functions[i].func(args);
            *argend = ')';
            return ret;
        }
    }

    return ERR_SYNTAX;
}

void uncomment(char *text) {
    #ifdef DEBUG
    debug_log("Removing comments...");
    #endif // DEBUG

    char *ptr = text;
    while((ptr = strchr(ptr, '#'))) {
        if(ptr > text) {
            if(ptr[-1] == '\\') {
                memmove(ptr - 1, ptr, strlen(ptr)+1);
                ++ptr;

                continue;
            }
        }

        char *ptr2 = strchr(ptr, '\n');
        if(ptr2 != NULL)
            memmove(ptr, ptr2 + 1, strlen(ptr2));
        else
            *ptr = 0;

        ++ptr;
    }
}
