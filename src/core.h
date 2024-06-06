#pragma once

#ifndef CORE_H
#define CORE_H

#include "variables.h"

Variable *eval(char *expression);

int run_line(char *code);

void uncomment(char *text);


#endif // CORE_H
