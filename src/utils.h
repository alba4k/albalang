#ifndef UTILS_H
#define UTILS_H

#include "variables.h"

char *skip_while(char *ptr);
char *skip_full(char *ptr);

void error(const char *message, const char *extra, const int code, void *memory, Number *num_head, String *str_head);

void uncomment(char *text);

int eval(const char *code);

#endif // UTILS_H
