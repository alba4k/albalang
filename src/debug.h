#pragma once

#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

extern bool debug;

void debug_log(const char *format, ...);

#endif // DEBUG_H
