#ifndef UTILS_H
#define UTILS_H

#include "variables.h"

char *skip_whites(char *ptr);
char *skip_full(char *ptr);

Variable *eval(char *expression);

void uncomment(char *text);

int run_line(char *code);

#endif // UTILS_H
