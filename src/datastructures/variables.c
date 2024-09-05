#include <stdlib.h>
#include <string.h>

#include "variables.h"
#include "../utils.h" // Don't really like relying on this bus ok (just for find_var)

Variable var_head = {
    NULL,
    Unassigned,
    {
        0
    },
    NULL,
    NULL,
};

#ifdef DEBUG
#include "../debug.h"
#endif // DEBUG

Variable *create_var(char *name, VariableType type, VariableValue value) {
    Variable *new = malloc(sizeof(Variable));

    if(new == NULL)
        return NULL;

    memset(new, 0, sizeof(Variable));

    #ifdef DEBUG
    if(type == Number)
        debug_log("Creating variable %s (%p, num: %f)", name, new, value.number);
    else if(type == String)
        debug_log("Creating variable %s (%p, str: %s)", name, new, value.string);
    else
        debug_log("Creating variable %s (%p, unassigned type)", name, new);
    #endif // DEBUG

    if(name != NULL) {
        new->name = malloc(sizeof(name)+1);

        if(new->name == NULL) {
            free(new);
            return NULL;
        }

        strcpy(new->name, name);
    }

    new->type = type;

    if(type == Number)
        new->value.number = value.number;
    else if(type == String) {
        new->value.string = malloc(strlen(value.string)+1);

        if(new->value.string == NULL) {
            free(new);
            return NULL;
        }

        strcpy(new->value.string, value.string);
    }

    return new;
}

int del_var(Variable *var) {
    if(var == NULL)
        return -1;

    #ifdef DEBUG
    debug_log("Deleting variable %s (%p, prev: %p; next: %p)", var->name, var, var->prev, var->next);
    #endif // DEBUG


    if(var->prev != NULL)
        var->prev->next = var->next;
    if(var->next != NULL)
        var->next->prev = var->prev;

    free(var->name);
    if(var->type == String)
        free(var->value.string);
    free(var);

    return 0;
}

Variable *edit_var(Variable *var, VariableType new_type, VariableValue value) {
    if(var == NULL)
        return NULL;

    #ifdef DEBUG
    if(var->type == String && new_type == String)
        debug_log("Changing variable %s (%p, %s -> %s)", var->name, var, var->value.string, value.string);
    else if(var->type == Number && new_type == String)
        debug_log("Changing variable %s (%p, %f -> %s)", var->name, var, var->value.number, value.string);
    else if(var->type == String && new_type == Number)
        debug_log("Changing variable %s (%p, %s -> %f)", var->name, var, var->value.string, value.number);
    else if(var->type == Number && new_type == Number)
        debug_log("Changing variable %s (%p, %f -> %f)", var->name, var, var->value.number, value.number);
    else if(var->type == Unassigned && new_type == Number)
        debug_log("Changing variable %s (%p, [] -> %f)", var->name, var, value.number);
    else if(var->type == Unassigned && new_type == String)
        debug_log("Changing variable %s (%p, [] -> %s)", var->name, var, value.string);
    #endif // DEBUG

    if(new_type == Number) {
        if(var->type == String)
            free(var->value.string);

        var->value.number = value.number;
    }
    else if(new_type == String) {
        if(var->type == String)
            var->value.string = realloc(var->value.string, strlen(value.string)+1);
        else
            var->value.string = malloc(strlen(value.string)+1);

        if(var->value.string == NULL) {
            del_var(var);
            return NULL;
        }

        strcpy(var->value.string, value.string);
    }
    else if(new_type == Unassigned) {
        if(var->type == String)
            free(var->value.string);
    }

    var->type = new_type;
    
    return var;
}

Variable *find_var(Variable *head, char *name) {
    if(head == NULL || name == NULL)
        return NULL;

    name = skip_whites(name);
    char *end = skip_full(name);
    char cache;
    if(end != NULL) {
        cache = *end;
        *end = 0;
    }

    Variable *current;
    for(current = head->next; current != NULL; current = current->next) {
        if(strcmp(current->name, name) == 0)
            break;
    }

    if(end != NULL)
        *end = cache;

    return current;
}

Variable *move_var(Variable *head, Variable *new) {
    if(head == NULL || new == NULL)
        return NULL;

    #ifdef DEBUG
        debug_log("Moving variable %s (%p) to stack %s", new->name, new, (head->name == NULL) ? "\e[1mdefault\e[0m" : head->name);
    #endif // DEBUG

    // reach the end of the linked list
    Variable *last = head;

    while(last->next != NULL)
        last = last->next;

    // remove new from a previous stack, if there is one
    if(new->prev != NULL)
        new->prev->next = new->next;
    if(new->next != NULL)
        new->next->prev = new->prev;

    last->next = new;
    new->next = NULL;
    new->prev = last;

    return new;
}
