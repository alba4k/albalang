#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib.h"
#include "utils.h"

#ifdef DEBUG
#include "debug.h"
#endif // DEBUG

// return a section of the heap containing a cleaner version of str
char *clean_string(char *str) {
    str = skip_whites(str);
    if(str == NULL)
        return NULL;

    char *start = malloc(strlen(str) + 1);
    if(start == NULL)
        return NULL;
    
    strcpy(start, str);

    char *end = start + strlen(start);
    while(end[-1] == ' ' || end[-1] == '\n' || end[-1] == '\t')
        --end;

    *end = 0;

    return start;
}

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

// divide arguments from a given strings into different elements.
// return NULL if or if no args were given
char **parse_args(char *str) {
    int argc = 0;
    char **argv = malloc(sizeof(char*));
    if(argv == NULL)
        return NULL;

    char *buf = clean_string(str);
    if(buf[0] == 0) {
        free(argv);
        return NULL;
    }

    char *start = buf;
    char *end;
    do {
        end = start;
        while((end = strchr(end, ','))) {
            if(is_in_string(buf, end) == false) {
                *end = 0;
                break;
            }
            ++end;
        }
        
        ++argc;

        argv = realloc(argv, (argc+1)*sizeof(char*));
        if(argv == NULL) {
            free(buf);
            return NULL;
        }

        argv[argc-1] = clean_string(start);
        if(argv[argc-1] == NULL) {
            free(buf);
            for(int i = 0; i < argc-1; ++i)
                free(argv[i]);
            free(argv);
            return NULL;
        }
        start = end+1;
    }
    while(end != NULL);

    argv[argc] = NULL;

    free(buf);
    return argv;
}

// skip every whitespace, return the first full character
char *skip_whites(char *ptr) {
    if(ptr == NULL)
        return NULL;
    if(ptr[0] == 0)
        return NULL;

    while(*ptr) {
        if(*ptr != ' ' && *ptr != '\n' && *ptr != '\t')
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
        if(*ptr == ' ' || *ptr == '\n' || *ptr == '\t')
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
