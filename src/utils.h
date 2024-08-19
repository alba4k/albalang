#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

#include "datastructures/variables.h"

char *clean_string(char *str);

char *find_section_end(const char *start);

bool is_in_string(const char *str, const char *place);

char **parse_args(char *str);

char *skip_whites(char *ptr);
char *skip_full(char *ptr);

void uncomment(char *text);

#endif // UTILS_H
