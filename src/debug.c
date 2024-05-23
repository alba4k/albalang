#include <stdio.h>
#include <stdarg.h>

void debug_log(const char *format, ...) {
    va_list args;
    va_start(args, format);

    fflush(stdout);

    fprintf(stderr, "[\e[1m\e[33mLOG\e[37m\e[0m]   ");
    vprintf(format, args);
    printf("\n");

    va_end(args);
}