#include <stdlib.h>
#include <string.h>

#include "variables.h"

Variable var_head = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

#ifdef DEBUG
#include "../debug.h"
#endif // DEBUG

Variable *add_var(Variable *head, Variable *new) {
    if(head == NULL || new == NULL)
        return NULL;

    #ifdef DEBUG
        debug_log("Adding variable %s to stack %s", new->name, (head->name == NULL) ? "\e[1mdefault\e[0m" : head->name);
    #endif // DEBUG

    // reach the end of the linked list
    Variable *last = head;

    while(last->next != NULL)
        last = last->next;

    // remove new from a previous stack, if there is one
    if(new->prev != NULL)
        new->prev->next = new->next;

    new->next = NULL;
    new->prev = last;
    last->next = new;

    return new;
}

Variable *create_var(char *name, double *num, char *str) {
    Variable *new = malloc(sizeof(Variable));
    memset(new, 0, sizeof(Variable));

    #ifdef DEBUG
    if(num != NULL)
        debug_log("Creating variable %s (%p, num: %f)", name, new, *num);
    else
        debug_log("Creating variable %s (%p, str: %s)", name, new, str);
    #endif // DEBUG

    if(name != NULL) {
        new->name = malloc(sizeof(name)+1);
        strcpy(new->name, name);
    }

    if(num != NULL) {
        new->number = malloc(sizeof(double));
        *(new->number) = *num;
    }
    else if(str != NULL) {
        new->string = malloc(strlen(str)+1);
        strcpy(new->string, str);
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
    free(var->number);
    free(var->string);
    free(var);

    return 0;
}

Variable *edit_var(Variable *var, double *num, char *str) {
    #ifdef DEBUG
    debug_log("Changing variable %s (%p, %f -> %f or %s -> %s)", var->name, var, var->number ? *(var->number) : 0, num ? *num : 0, var->string, str);
    #endif // DEBUG

    if(var == NULL)
        return NULL;

    if(num != NULL) {
        if(var->number == NULL)
            var->number = malloc(sizeof(double));
        *(var->number) = *(num);

        free(var->string);
        var->string = NULL;
    }
    else if(str != NULL) {
        var->string = realloc(var->string, strlen(str)+1);
        strcpy(var->string, str);

        free(var->number);
        var->number = NULL;
    }
    
    return var;
}

Variable *find_var(Variable *head, char *name) {
    for(Variable *current = head->next; current != NULL; current = current->next) {
        if(strcmp(current->name, name) == 0)
            return current;
    }

    return NULL;
}
