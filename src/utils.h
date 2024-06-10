#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#include "variables.h"

char *find_section_end(const char *start);

bool is_in_string(const char *str, const char *place);

char *skip_whites(char *ptr);
char *skip_full(char *ptr);

void uncomment(char *text);

#endif // UTILS_H
