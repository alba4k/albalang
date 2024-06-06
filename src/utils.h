#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#include "variables.h"

bool is_in_string(const char *str, const char *place);

char *skip_whites(char *ptr);
char *skip_full(char *ptr);

#endif // UTILS_H
