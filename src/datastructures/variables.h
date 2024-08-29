#pragma once

#ifndef VARIABLES_H
#define VARIABLES_H

typedef enum VariableType {
    Unassigned = 0,
    Number = 1,
    String = 2,
} VariableType;

typedef union VariableValue {
    double number;
    char *string;
} VariableValue;

typedef struct Variable {
    char *name;
    VariableType type;

    VariableValue value;

    struct Variable *prev;
    struct Variable *next;
} Variable;

// heads of the linked list, defined in variables.c
extern Variable var_head;

Variable *create_var(char *name, VariableType type, VariableValue value);

int del_var(struct Variable *var);

Variable *edit_var(Variable *var, VariableType new_type, VariableValue value);

Variable *find_var(Variable *head, char *name);

Variable *move_var(struct Variable *head, Variable *new);

#endif // VARIABLES_H
