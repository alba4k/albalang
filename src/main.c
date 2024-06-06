#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "core.h"
#include "error.h"
#include "utils.h"
#include "variables.h"

int main(int argc, char **argv) {
    // guards against incorrect usage
    if(argc != 2)
        error("Usage: albalang <file>.al.", NULL, ERR_BAD_USAGE, NULL);

    if(access(argv[1], F_OK))
        error("This file does not exist:", argv[1], ERR_NO_SUCH_FILE, NULL);

    // open the file
    FILE *fp = fopen(argv[1], "r");
    if(!fp)
        error("The file could not be opened.", argv[1], ERR_FILE_UNREADABLE, NULL);

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
        if(endline > code) {
            // \; should not end a line
            if(is_in_string(code, endline) == true) {
                endline = strchr(endline+1, ';');

                if(endline == NULL)
                    break;
            }
        }
        
        *endline = 0;
        int ret = run_line(line);
        if(ret != 0) {
            *endline = ';';

            // line formatting
            line = skip_whites(line);
            char *ptr;
            if((ptr = strchr(line, ';')))
                ptr[1] = 0;

            error("An error occurred while running the following line", skip_whites(line), ret, code);
        }

        *endline = ';';
        ++endline;
        line = endline;
    }

    // clear variable linked list
    while(var_head.next != NULL)
        del_var(var_head.next);

    free(code);

    return 0;
}