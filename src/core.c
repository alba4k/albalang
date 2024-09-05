#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "core.h"
#include "error.h"
#include "stdlib.h"
#include "utils.h"
#include "datastructures/lists.h"
#include "datastructures/variables.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

Variable *eval(char *expression) {
    #ifdef DEBUG
    debug_log("Trying to evaluate `%s`", expression);
    #endif // DEBUG

    /* 
     * This can currently parse:
     * ${var}
     * "text"
     * 4.5
     * list[${num}]
     */

    VariableValue value = {0};
    Variable *result = create_var(NULL, Unassigned, value);

    if(result == NULL)
        return NULL;
    
    expression = skip_whites(expression);
            
    if(expression == NULL)
        return result;

    char *end;

    if(expression[0] == '$') {
        if(expression[1] != '{')
            return result;
        
        end = strchr(expression+2, '}');
        if(end == NULL)
            return result;
            
        *end = 0;

        Variable *var = find_var(&var_head, expression+2);

        *end = '}';

        if(var == NULL)
            return result;

        result->name = realloc(result->name, strlen(var->name) + 1);

        if(result->name == NULL) {
            del_var(result);
            return NULL;
        }

        strcpy(result->name, var->name);

        edit_var(result, var->type, var->value);
    }
    else if(expression[0] == '"') {
        end = strchr(expression+1, '"');
        if(end == NULL)
            return result;
        
        *end = 0;
        value.string = expression+1;
        edit_var(result, String, value);
        *end = '"';
    }
    else if((end = strchr(expression, '['))) {
        char *end2 = strchr(end, ']');
        if(end2 == NULL)
            return result;

        *end = 0;
        *end2 = 0;
        Variable *var = eval(end+1); // recursion? ehhh
        *end2 = ']';

        if(var == NULL) {
            del_var(var);
            *end = '[';
            return NULL;
        }
        if(var->type != Number) {
            del_var(var);
            *end = '[';
            return result;
        }

        List *list = find_list(&list_head, expression);
        *end = '[';
        
        if(list == NULL)
            return result;
        
        Variable *current = list->head.next;
        for(int i = 0; i < (int)var->value.number && current != NULL; ++i)
            current = current->next;

        del_var(var);

        if(current == NULL)
            return result;
        
        result->name = realloc(result->name, strlen(current->name) + 1);

        if(result->name == NULL) {
            del_var(result);
            return NULL;
        }

        strcpy(result->name, current->name);

        edit_var(result, current->type, current->value);
    }
    else {
        value.number = atof(expression);

        edit_var(result, Number, value);
    }

    return result;
}

int run_code(char *code) {
    char *endline = code;
    char *line = code;

    while((endline = strchr(line, ';')) != NULL) {
        if(endline > code) {
            while(is_in_string(code, endline) == true) {
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
            bool inverted = false;

            if(strncmp(ptr, "not", 3) == 0) {
                #ifdef DEBUG
                debug_log("Inverting the if condition");
                #endif // DEBUG

                inverted = true;
                
                ptr = skip_whites(ptr+3);
            }

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

            if(var == NULL)
                error("Not enough memory", code, ERR_OOM, NULL);

            if(var->type == String) {
                del_var(var);
                ptr = strchr(line, '\n');
                ptr[0] = 0;
                error("if requires a number, not a string", line, ERR_WRONG_TYPE, code);
            }

            if(var->type != Number) {
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

            if(inverted ? (var->value.number == 0) : (var->value.number != 0)) {
                ptr[0] = 0;
                int ret = run_code(start+1);  // recursion, meh
                ptr[0] = '}';
                
                if(ret != RET_OK) {
                    del_var(var);
                    return ret;
                }

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
                        ++ptr;
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
                int ret = run_code(ptr+1);  // recursion, meh
                start[0] = '}';

                if(ret != RET_OK)
                    return ret;

                ptr = start + 1;
            }

            end:
            line = ptr;
            del_var(var);
            continue;
        }

        // use recursive calls to run the block relative to the while statement
        if(strncmp(line, "while", 5) == 0) {
            #ifdef DEBUG
            debug_log("Found a while statement");
            #endif // DEBUG

            *endline = ';';

            char *condition = skip_whites(line+5);
            char *start = condition;
            char *end;
            bool inverted = false;

            if(strncmp(condition, "not", 3) == 0) {
                #ifdef DEBUG
                debug_log("Inverting the if condition");
                #endif // DEBUG

                inverted = true;
                
                condition = skip_whites(condition+3);
            }

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
                if(var->type == String) {
                    del_var(var);
                    end = strchr(line, '\n');
                    end[0] = 0;
                    error("while requires a number, not a string", line, ERR_WRONG_TYPE, code);
                }

                if(var->type != Number) {
                    del_var(var);
                    end = strchr(line, '\n');
                    end[0] = 0;
                    error("while requires a number", line, ERR_GENERIC, code);
                }
                
                if(inverted ? (var->value.number != 0) : (var->value.number == 0)) {
                    del_var(var);
                    break;
                }

                int ret = run_code(start+1);
                del_var(var);

                if(ret == RET_CONTINUE)
                    continue;
                if(ret == RET_BREAK)
                    break;
            }
            if(var == NULL)
                error("Not enough memory", code, ERR_OOM, NULL);

            start[0] = '{';
            end[0] = '}';
            
            line = end+1;
            continue;
        }
        // use a recursive call to run the code contained by the specified file
        if(strncmp(line, "include", 7) == 0) {
            #ifdef DEBUG
            debug_log("Found a file to include");
            #endif // DEBUG

            char *ptr = skip_whites(line+7);

            if(ptr == NULL)
                error("no file to include was specified", line, ERR_SYNTAX, code);

            Variable *var = eval(ptr);

            if(var == NULL)
                error("Not enough memory", code, ERR_OOM, NULL);

            if(var->type == Number) {
                del_var(var);
                error("include requires a string, not a number", line, ERR_WRONG_TYPE, code);
            }
            if(var->type != String) {
                del_var(var);
                error("include requires a string", line, ERR_GENERIC, code);
            }

            #ifdef DEBUG
            debug_log("Trying to run the contents of %s via include");
            #endif // DEBUG

            run_file(var->value.string);

            del_var(var);
        }
        else if(strncmp(line, "continue", 8) == 0) {
            #ifdef DEBUG
            debug_log("Found a continue statement");
            #endif // DEBUG

            *endline = ';';
            return RET_CONTINUE;
        }
        else if(strncmp(line, "break", 5) == 0) { 
            #ifdef DEBUG
            debug_log("Found a break statement");
            #endif // DEBUG

            *endline = ';';
            return RET_BREAK;
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

    return RET_OK;
}

void run_file(char *filename) {
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
    if(code == NULL)
        error("Not enough memory", filename, ERR_OOM, NULL);

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

    // we have something like `var   variable = 80.9  `
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
        if(value == NULL)
            return ERR_OOM;
        
        // read ` 80.9`

        // they are both NULL
        if(value->type == Unassigned) {
            if(ptr2 != NULL)
                *ptr2 = cache;
            return ERR_SYNTAX;
        }

        Variable *var = find_var(&var_head, name);
        if(var == NULL) {
            Variable *new = create_var(name, value->type, value->value);

            if(new == NULL)
                return ERR_OOM;

            move_var(&var_head, new);
        }
        else
            edit_var(var, value->type, value->value);

        del_var(value);

        if(ptr2 != NULL)
            *ptr2 = cache;

        return 0;
    }
    // we have something like `list L << var  `
    else if(strncmp(code, "list ", 5) == 0) {
        ptr = skip_whites(code+5);
        if(ptr == NULL)
            return ERR_SYNTAX;

        // `L << var  `

        char *ptr2;
        char *end = skip_full(ptr);
        char cache = 0;

        if((ptr2 = strstr(ptr, "<<"))) {
            if(end == NULL || ptr2+1 == end)
                end = ptr2;
            cache = *end;
            *end = 0;

            List *list = find_list(&list_head, ptr);

            if(list == NULL) {
                list = create_list(ptr);

                if(list == NULL)
                    return ERR_OOM;

                add_list(&list_head, list);
            }

            *end = cache;

            ptr2 += 2;

            // `var  `

            ptr2 = skip_whites(ptr2);
            if(ptr2 == NULL)
                return ERR_SYNTAX;

            Variable *var = find_var(&var_head, ptr2);
            if(var == NULL)
                return ERR_VAR_NOT_FOUND;
            Variable *list_var = find_var(&(list->head), ptr2);

            if(list_var != NULL) {
                edit_var(list_var, var->type, var->value);

                del_var(var);
            }
            else
                move_var(&(list->head), var);
        }
        else if((ptr2 = strstr(ptr, ">>"))) {
            if(end == NULL || ptr2+1 == end)
                end = ptr2;
            cache = *end;
            *end = 0;

            List *list = find_list(&list_head, ptr);

            *end = cache;

            if(list == NULL)
                return ERR_LIST_NOT_FOUND;

            ptr2 += 2;

            ptr2 = skip_whites(ptr2);
            if(ptr2 == NULL)
                return ERR_SYNTAX;

            Variable *list_var = find_var(&(list->head), ptr2);
            if(list_var == NULL)
                return ERR_VAR_NOT_FOUND;
            Variable *var = find_var(&var_head, ptr2);
            if(var != NULL) {
                edit_var(var, list_var->type, list_var->value);

                del_var(list_var);
            }
            else
                move_var(&var_head, list_var);
        }
        else
            return ERR_SYNTAX;

        return 0;
    }

    // functions belonging to stdlib
    for(size_t i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        if(strncmp(code, functions[i].name, strlen(functions[i].name)) == 0) {
            ptr = code + strlen(functions[i].name);
            
            char *argstr = skip_whites(ptr);
            if(argstr[0] != '(')
                return ERR_SYNTAX;
            ++argstr;
            
            char *argend = strchr(argstr, ')');
            if(argend == NULL)
                return ERR_SYNTAX;
                
            *argend = 0;
            char **argv = parse_args(argstr);
            *argend = ')';
            if(argv == NULL)
                return ERR_GENERIC;
            
            int ret = functions[i].func(argv);

            for(int i = 0; argv[i] != 0; ++i)
                free(argv[i]);
            free(argv);

            return ret;
        }
    }

    return ERR_SYNTAX;
}
