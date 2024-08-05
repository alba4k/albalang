#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

// find the closing } to a section starting with { and return the following character
char *find_section_end(const char *start) {
    if(start[0] != '{')
        return NULL;

    int count = 1;
    char *ptr = (char*)start;

    do {
        ++ptr;

        if(ptr[0] == '{')
            ++count;
        else if(ptr[0] == '}')
            --count;
    } while(count > 0 && ptr[0] != 0);

    if(ptr[0] == 0)
        return NULL;
    else
        return ptr;
}

// checks if the given character is between two " "
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

// skip every whitespace, return the first full character
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

// skip every full character, return the first whitespace
char *skip_full(char *ptr) {
    if(ptr == NULL)
        return NULL;

    while(*ptr) {
        if(*ptr == ' ' || *ptr == '\n')
            return ptr;
        ++ptr;
    }

    return NULL;
}

void uncomment(char *text) {
    #ifdef DEBUG
    debug_log("Removing comments...");
    #endif // DEBUG

    char *ptr = text;
    while((ptr = strchr(ptr, '#'))) {
        if(is_in_string(text, ptr) == true) {
            ++ptr;
            continue;
        }

        char *ptr2 = strchr(ptr, '\n');
        if(ptr2 != NULL)
            memmove(ptr, ptr2 + 1, strlen(ptr2));
        else
            *ptr = 0;

        ++ptr;
    }
}
