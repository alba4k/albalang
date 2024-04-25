#include <stdlib.h>
#include <string.h>

#include "variables.h"

Variable var_head = {
    "",
    NULL,
    NULL,
    NULL,
    NULL,
};

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

Variable *add_var(Variable *head, char *name, double *num, char *str) {
    // reach the end of the linked list
    Variable *last = head;

    Variable *new = malloc(sizeof(Variable));

    #ifdef DEBUG
    debug_log("Creating variable %s (%p, value: %f or %s)", name, new, num ? *num : 0, str);
    #endif // DEBUG

    new->name = malloc(sizeof(name)+1);
    strcpy(new->name, name);

    new->number = NULL;
    new->string = NULL;

    if(str != NULL) {
        new->string = malloc(strlen(str)+1);
        strcpy(new->string, str);
    }
    else if(num != NULL) {
        new->number = malloc(sizeof(double));
        *(new->number) = *num;
    }

    while(last->next != NULL)
        last = last->next;

    new->next = NULL;
    new->prev = last;
    last->next = new;

    return new;
}

Variable *edit_var(Variable *var, char *str, double *num) {
    #ifdef DEBUG
    debug_log("Changing variable %s (%p, %f -> %f or %s -> %s)", var->name, var, var->number ? *(var->number) : 0, num ? *num : 0, var->string, str);
    #endif // DEBUG

    if(num != NULL) {
        var->string = realloc(var->string, strlen(str)+1);
        strcpy(var->string, str);
    }
    else if(num != NULL) {
        *(var->number) = *(num);
    }

    return var;
}

int del_var(Variable *var) {
    if(var == NULL)
        return -1;

    #ifdef DEBUG
    debug_log("Deleting variable %s (%p, prev: %p; next: %p)", var->name, var, var->prev, var->next);
    #endif // DEBUG

    var->prev->next = var->next;
    if(var->next != NULL)
        var->next->prev = var->prev;

    free(var->name);
    free(var->number);
    free(var->string);
    free(var);

    return 0;
}

Variable *find_var(Variable *head, char *name) {
    for(Variable *current = head->next; current != NULL; current = current->next) {
        if(strcmp(current->name, name) == 0)
            return current;
    }

    return NULL;
}
