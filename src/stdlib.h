#pragma once

#ifndef STDLIB_H
#define STDLIB_H

struct Function {
    const char *name;
    int (*func)(char **);
};

extern const struct Function functions[14];

#endif // STDLIB_H
