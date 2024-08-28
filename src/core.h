#pragma once

#ifndef CORE_H
#define CORE_H

#include "datastructures/variables.h"

Variable *eval(char *expression);

int run_code(char *code);

void run_file(char *filename);

int run_line(char *code);

#endif // CORE_H
