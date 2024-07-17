#pragma once

#ifndef STDLIB_H
#define STDLIB_H

struct function {
    const char *name;
    int (*func)(char *);
};

extern const struct function functions[11];

#endif // STDLIB_H
