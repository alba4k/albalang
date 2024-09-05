#include <stdlib.h>
#include <string.h>

#include "lists.h"
#include "variables.h"
#include "../utils.h" // Don't really like relying on this bus ok (just for find_var)

List list_head = {
    NULL,
    {
        NULL,
        Unassigned,
        {
            0
        },
        NULL,
        NULL,
    },
    NULL,
    NULL
};

#ifdef DEBUG
#include "../debug.h"
#endif // DEBUG

List *add_list(struct List *head, List *new) {
    if(head == NULL || new == NULL) {
        del_list(new);
        return NULL;
    }

    #ifdef DEBUG
        debug_log("Adding list %s to stack %s", new->name, (head->name == NULL) ? "\e[1mdefault\e[0m" : head->name);
    #endif // DEBUG

    if(new->prev != NULL)
        new->prev->next = new->next;
    if(new->next != NULL)
        new->next->prev = new->prev;

    // reach the end of the linked list
    List *last = head;

    while(last->next != NULL)
        last = last->next;

    new->next = NULL;
    new->prev = last;
    last->next = new;

    return new;
}

List *create_list(char *name) {
    List *new = malloc(sizeof(List));
    
    if(new == NULL)
        return NULL;

    memset(new, 0, sizeof(List));

    #ifdef DEBUG
        debug_log("Creating list %s (%p)", name, new);
    #endif // DEBUG

    if(name != NULL) {
        new->name = malloc(strlen(name)+1);
    
        if(new->name == NULL) {
            free(new);
            return NULL;
        }

        new->head.name = new->name;

        strcpy(new->name, name);
    }

    return new;
}

int del_list(struct List *list) {
    if(list == NULL)
        return -1;

    #ifdef DEBUG
    debug_log("Deleting list %s (%p, prev: %p; next: %p)", list->name, list, list->prev, list->next);
    #endif // DEBUG

    if(list->prev != NULL)
        list->prev->next = list->next;
    if(list->next != NULL)
        list->next->prev = list->prev;

    free(list->name);
    
    // clear variable linked list
    while(list->head.next != NULL)
        del_var(list->head.next);
        
    free(list);

    return 0;
}

List *find_list(List *head, char *name) {
    if(head == NULL || name == NULL)
        return NULL;

    name = skip_whites(name);
    char *end = skip_full(name);
    char cache;
    if(end != NULL) {
        cache = *end;
        *end = 0;
    }

    List *current;
    for(current = head->next; current != NULL; current = current->next) {
        if(strcmp(current->name, name) == 0)
            break;
    }

    if(end != NULL)
        *end = cache;

    return current;
}
