#pragma once

#ifndef VARIABLES_H
#define VARIABLES_H

struct Variable {
    char *name;

    double *number;
    char *string;

    struct Variable *prev;
    struct Variable *next;
};

typedef struct Variable Variable;

// heads of the linked list, defined in variables.c
extern Variable var_head;

Variable *move_var(struct Variable *head, Variable *new);

Variable *create_var(char *name, double *num, char *str);

int del_var(struct Variable *var);

Variable *edit_var(Variable *var, double *num, char *str);

Variable *find_var(Variable *head, char *name);

#endif // VARIABLES_H
