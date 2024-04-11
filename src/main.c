#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "variables.h"

int main(int argc, char **argv) {
    // guards against incorrect usage
    if(argc < 2)
        error("Usage: albalang <file>.al.", NULL, 1, NULL, NULL, NULL);

    if(access(argv[1], F_OK))
        error("This file does not exist:", argv[1], 2, NULL, NULL, NULL);

    // open the file
    FILE *fp = fopen(argv[1], "r");
    if(!fp)
        error("The file could not be opened.", argv[1], 3, NULL, NULL, NULL);

    // get the size of the file
    fseek(fp, 0, SEEK_END);
    size_t len = (size_t)ftell(fp);
    rewind(fp);
    
    // read the file
    char *code = malloc(len+1);
    code[fread(code, 1, len, fp)] = 0;
    fclose(fp);

    uncomment(code);

    char *endline = code;
    char *line = code;
    while((endline = strchr(endline, ';')) != NULL) {
        *endline = 0;

        if(eval(line) != 0) {
            *endline = ';';

            // line formatting
            line = skip_white(line);
            char *ptr;
            if((ptr = strchr(line, ';')))
                ptr[1] = 0;


            error("And error occurred while running the following line:", line, -1, code, num_head.next, str_head.next);
        }

        *endline = ';';
        ++endline;
        line = endline;
    }

    // clear linked lists
    Number *n_current = num_head.next;
    while(n_current != NULL) {
        Number *next = n_current->next;
        free(n_current);
        n_current = next;
    }
    String *s_current = str_head.next;
    while(s_current != NULL) {
        String *next = s_current->next;
        free(s_current->value);
        free(s_current);
        s_current = next;
    }
    free(code);

    return 0;
}