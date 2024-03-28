#pragma once

#ifndef STDLIB_H
#define STDLIB_H

struct function {
    char *name;
    const int (*func)(char *);
};

extern const struct function functions[1];

#endif // STDLIB_H
