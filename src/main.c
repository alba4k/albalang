#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "core.h"
#include "debug.h"
#include "error.h"
#include "datastructures/variables.h"
#include "datastructures/lists.h"

bool debug = false;

int main(int argc, char **argv) {
    // TODO: Proper argument parsing
    if(argc >= 2)
        if(strcmp(argv[1], "--debug") == 0)
            debug = true;

    // guards against incorrect usage
    if(argc != (debug ? 3 : 2))
        error("Usage: albalang [--debug] <file>.al.", NULL, ERR_BAD_USAGE, NULL);

    run_file(argv[debug ? 2 : 1]);

    // clear variable linked list
    while(var_head.next != NULL)
        del_var(var_head.next);
    
    // clear list linked list
    while(list_head.next != NULL)
        del_list(list_head.next);

    return 0;
}
