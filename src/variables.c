#include <stdlib.h>
#include <string.h>

#include "variables.h"

Number num_head = {
    "",
    0,
    NULL,
    NULL,
};
String str_head = {
    "",
    NULL,
    NULL,
    NULL,
};

Number *add_num(Number *head, char *name, double value) {
    // reach the end of the linked list
    Number *last = head;

    while(last->next != NULL)
        last = last->next;

    Number *new = malloc(sizeof(Number));

    strncpy(new->name, name, sizeof(new->name));
    new->value = value;

    new->next = NULL;
    if(last) {
        new->prev = last;
        last->next = new;
    }
    else
        new->prev = NULL;

    return new;
}

String *add_str(String *head, char *name, char *value) {
    // reach the end of the linked list
    String *last = head;

    while(last->next != NULL)
        last = last->next;
            
    String *new = malloc(sizeof(String));

    strncpy(new->name, name, sizeof(new->name));

    size_t len = strlen(value);
    new->value = malloc(len+1);
    strcpy(new->value, value);

    new->next = NULL;
    new->prev = last;
    last->next = new;

    return new;
}

Number *edit_num(Number *var, double value) {
    var->value = value;

    return var;
}

String *edit_str(String *var, char *value) {
    size_t len = strlen(value);
    var->value = realloc(var->value, len+1);
    strcpy(var->value, value);

    return var;
}

Number *del_num(Number *var) {
    Number *prev = var->prev;

    // var->prev->next = var->next;
    var->next = var->next;

    free(var);
    
    return 0;
}

String *del_str(String *var) {
    String *prev = var->prev;

    prev->next = var->next;

    free(var->value);
    free(var);

    return 0;
}

Number *find_num(Number *head, char *name) {
    for(Number *current = head->next; current != NULL; current = current->next) {
        if(strcmp(current->name, name) == 0)
            return current;
    }

    return NULL;
}

String *find_str(String *head, char *name) {
    for(String *current = head->next; current != NULL; current = current->next) {
        if(strcmp(current->name, name) == 0)
            return current;
    }

    return NULL;
}
