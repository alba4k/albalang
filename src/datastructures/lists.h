#pragma once

#ifndef LISTS_H
#define LISTS_H

#include "variables.h"

/* 
 * When an existing variable is added to a list,
 * the list takes ownership of that variable.
 * This means that the variable is removed from the
 * stack that previously owned it and added to
 * the stack created inside of the list. The memory
 * containing the variable is left untouched but
 * the pointer is now stored in another location.
 * You cannot add a list to another list.
 */

struct List
{
    char *name;

    Variable *first_var;

    struct List *prev;
    struct List *next;
};

typedef struct List List;

// heads of the linked list, defined in lists.c
extern List list_head;

List *add_list(struct List *head, List *new);

List *create_list(char *name);

int del_list(struct List *list);

List *find_list(List *head, char *name);

#endif // LISTS_H
