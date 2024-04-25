#ifndef UTILS_H
#define UTILS_H

#include "variables.h"

char *skip_whites(char *ptr);
char *skip_full(char *ptr);

void error(const char *message, const char *extra, const int code, void *memory);

void uncomment(char *text);

int eval(char *code);

#endif // UTILS_H
