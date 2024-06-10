#pragma once

#ifndef CORE_H
#define CORE_H

#include "variables.h"

Variable *eval(char *expression);

void run_code(char *code);

int run_line(char *code);

#endif // CORE_H
