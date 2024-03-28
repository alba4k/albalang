#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int main(int argc, char **argv) {
    // guards against incorrect usage
    if(argc < 2)
        error("Usage: albalang <file>.al.", NULL, 1, NULL);

    if(access(argv[1], F_OK))
        error("This file does not exist:", argv[1], 2, NULL);

    // open the file
    FILE *fp = fopen(argv[1], "r");
    if(!fp)
        error("The file could not be opened.", argv[1], 3, NULL);

    // get the size of the file
    fseek(fp, 0, SEEK_END);
    size_t len = (size_t)ftell(fp);
    rewind(fp);
    
    // read the file
    char *code = malloc(len+1);
    code[fread(code, 1, len, fp)] = 0;
    fclose(fp);

    char *endline = code;
    char *line = code;
    while((endline = strchr(endline, ';')) != NULL) {
        *endline = 0;

        if(eval(line) != 0) {
            *endline = ';';

            // line formatting
            while(line[0] == ' ' || line[0] == '\n')
                line++;
            char *ptr;
            if((ptr = strchr(line, ';')))
                ptr[1] = 0;


            error("And error occurred while running the following line:", line, -1, code);
        }

        *endline = ';';
        ++endline;
        line = endline;
    }

    free(code);

    return 0;
}