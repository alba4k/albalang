#ifndef VARIABLES_H
#define VARIABLES_H

struct Number {
    char name[32];
    double value;

    struct Number *next;
    struct Number *prev;
};

struct String {
    char name[32];
    char *value;

    struct String *next;
    struct String *prev;
};

typedef struct Number Number;
typedef struct String String;

// heads of the linked lists, defined in variables.c
extern struct Number num_head;
extern struct String str_head;

Number *add_num(struct Number *head, char *name, double value);

String *add_str(struct String *head, char *name, char *value);

Number *edit_num(struct Number *var, double value);

String *edit_str(String *var, char *value);

Number *del_num(struct Number *var);

String *del_str(struct String *var);

Number *find_num(Number *head, char *name);

String *find_str(String *head, char *name);

#endif // VARIABLES_H
