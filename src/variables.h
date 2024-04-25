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

// heads of the linked lists, defined in variables.c
extern Variable var_head;

Variable *add_var(struct Variable *head, char *name, double *num, char *str);

Variable *edit_var(Variable *var, char *str, double *num);

int del_var(struct Variable *var);

Variable *find_var(Variable *head, char *name);

#endif // VARIABLES_H
