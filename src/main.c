#include <stddef.h>

#include "core.h"
#include "error.h"
#include "datastructures/variables.h"

int main(int argc, char **argv) {
    // guards against incorrect usage
    if(argc != 2)
        error("Usage: albalang <file>.al.", NULL, ERR_BAD_USAGE, NULL);

    run_file(argv[1]);

    // clear variable linked list
    while(var_head.next != NULL)
        del_var(var_head.next);

    return 0;
}
