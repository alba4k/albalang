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


Variable *add_var(Variable *head, char *name, double *num, char *str) {
    // reach the end of the linked list
    Variable *last = head;

    Variable *new = malloc(sizeof(Variable));

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
