#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void run_code(char *code) {
    char *endline = code;
    char *line = code;

    while((endline = strchr(line, ';')) != NULL) {
        if(endline > code) {
            // \; should not end a line
            if(is_in_string(code, endline) == true) {
                endline = strchr(endline+1, ';');

                if(endline == NULL)
                    break;
            }
        }

        *endline = 0;
        line = skip_whites(line);
        if(line == NULL) {
            *endline = ';';
            line = endline+1;
            continue;
        }

        // use a recursive call to run the blocks relative to the if/else statement
        if(strncmp(line, "if", 2) == 0) {
            #ifdef DEBUG
            debug_log("Found an if statement");
            #endif // DEBUG

            *endline = ';';

            char *ptr = skip_whites(line+2);
            char *start = ptr;

            if(ptr[0] == '$')
                start = strchr(ptr+1,'}');

            start = strchr(start, '{');
            if(start == NULL) {
                ptr = strchr(line, '\n');
                ptr[0] = 0;
                error("if requires a starting '{'", line, ERR_SYNTAX, code);
            }
            start[0] = 0;

            Variable *var = eval(ptr);

            if(var->string != NULL) {
                del_var(var);
                ptr = strchr(line, '\n');
                ptr[0] = 0;
                error("if requires a number, not a string", line, ERR_WRONG_TYPE, code);
            }

            if(var->number == NULL) {
                del_var(var);
                ptr = strchr(line, '\n');
                ptr[0] = 0;
                error("if requires a number", line, ERR_GENERIC, code);
            }

            start[0] = '{';
            ptr = find_section_end(start);
            
            if(ptr == NULL) {
                del_var(var);
                ptr = strchr(line, '\n');
                ptr[0] = 0;
                error("if requires a closing '}'", line, ERR_SYNTAX, code);
            }

            if(*(var->number) != 0) {
                ptr[0] = 0;
                run_code(start+1);  // recursion, meh
                ptr[0] = '}';
                
                ++ptr;
                start = skip_whites(ptr);
                
                if(start != NULL)
                    if(strncmp(start, "else", 4) == 0) {
                        start = skip_whites(start+4);
                        ptr = find_section_end(start);
                        if(ptr == NULL) {
                            del_var(var);
                            ptr = strchr(line, '\n');
                            ptr[0] = 0;
                            error("else requires a starting '{' and a closing '}'", line, ERR_SYNTAX, code);
                        }
                    }
            }
            else {
                ++ptr;
                start = skip_whites(ptr);

                if(start == NULL)
                    goto end;
                if(strncmp(start, "else", 4) != 0)
                    goto end;
                start += 4;

                #ifdef DEBUG
                debug_log("Found an else statement");
                #endif // DEBUG

                ptr = skip_whites(start);

                start = find_section_end(ptr);
                if(start == NULL) {
                    del_var(var);
                    ptr = strchr(line, '\n');
                    ptr[0] = 0;
                    error("else also requires a starting { and a closing }", line, ERR_SYNTAX, code);
                }
                
                start[0] = 0;
                run_code(ptr);  // recursion, meh
                start[0] = '}';

                ptr = start + 1;
            }

            end:
            line = ptr;
            del_var(var);
            continue;
        }
        
        if(strncmp(line, "while", 5) == 0) {
            #ifdef DEBUG
            debug_log("Found a while statement");
            #endif // DEBUG

            *endline = ';';

            char *condition = skip_whites(line+5);
            char *start = condition;
            char *end;

            if(condition[0] == '$')
                start = strchr(condition+1,'}');

            start = strchr(start, '{');
            if(start == NULL) {
                end = strchr(line, '\n');
                end[0] = 0;
                error("while requires a starting '{'", line, ERR_SYNTAX, code);
            }
            
            end = find_section_end(start);
            if(end == NULL) {
                end = strchr(line, '\n');
                end[0] = 0;
                error("while requires a closing '}'", line, ERR_SYNTAX, code);
            }
            end[0] = 0;

            start[0] = 0;

            Variable *var;

            while((var = eval(condition))) {
                if(var->string != NULL) {
                    del_var(var);
                    end = strchr(line, '\n');
                    end[0] = 0;
                    error("while requires a number, not a string", line, ERR_WRONG_TYPE, code);
                }

                if(var->number == NULL) {
                    del_var(var);
                    end = strchr(line, '\n');
                    end[0] = 0;
                    error("while requires a number", line, ERR_GENERIC, code);
                }
                
                if(*(var->number) == 0) {
                    del_var(var);
                    break;
                }

                run_code(start+1);
                del_var(var);
            }

            end[0] = '}';
            start[0] = '{';
            
            line = end+1;
            continue;
        }

        // use a recursive call to run the code contained by the specified file
        if(strncmp(line, "include", 7) == 0) {
            char *ptr = skip_whites(line+7);

            if(ptr == NULL)
                error("no file to include was specified", line, ERR_SYNTAX, code);

            Variable *var = eval(ptr);

            if(var->number != NULL) {
                del_var(var);
                error("include requires a string, not a number", line, ERR_WRONG_TYPE, code);
            }

            if(var->string == NULL) {
                del_var(var);
                error("include requires a string", line, ERR_GENERIC, code);
            }

            #ifdef DEBUG
            debug_log("Trying to run the contents of %s via include");
            #endif // DEBUG

            run_file(var->string);
        }
        else {  // run the line using run_line
            int ret = run_line(line);

            if(ret != 0) {
                *endline = ';';

                // line formatting
                char *ptr;
                if((ptr = strchr(line, ';')))
                    ptr[1] = 0;

                error("An error occurred while running the following line", skip_whites(line), ret, code);
            }
        }

        *endline = ';';
        line = endline+1;
    }
}

void run_file(const char *filename) {
    if(access(filename, F_OK) != 0)
        error("This file does not exist in the current path", filename, ERR_NO_SUCH_FILE, NULL);

    // open the file
    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
        error("The file could not be opened.", filename, ERR_FILE_UNREADABLE, NULL);

    // get the size of the file
    fseek(fp, 0, SEEK_END);
    size_t len = (size_t)ftell(fp);
    rewind(fp);
            
    // read the file
    char *code = malloc(len+1);
    code[fread(code, 1, len, fp)] = 0;
    fclose(fp);

    uncomment(code);

    run_code(code);

    free(code);
}

int run_line(char *code) {
    if(code == NULL)
        return 0;

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
