#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

bool is_in_string(const char *str, const char *place) {
    int count = 0;

    char *ptr = (char*)str;
    while((ptr = strchr(ptr, '"')) && ptr < (char*)place) {
        ++count;
        ptr += 1;
    }

    if(count % 2 == 1)
        return true;

    return false;
}

char *skip_whites(char *ptr) {
    if(ptr == NULL)
        return NULL;
    if(ptr[0] == 0)
        return NULL;

    while(*ptr) {
        if(*ptr != ' ' && *ptr != '\n')
            return ptr;
        ++ptr;
    }

    return NULL;
}

char *skip_full(char *ptr) {
    if(ptr == NULL)
        return NULL;
    if(ptr[0] == 0)
        return NULL;

    while(*ptr) {
        if(*ptr == ' ' || *ptr == '\n')
            return ptr;
        ++ptr;
    }

    return NULL;
}
