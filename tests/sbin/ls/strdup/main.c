
#include <stdlib.h>

#include "tap.h"
#include "ls.h"

int
main (int argc, char **argv)
{
    char *res = NULL;

    res = strdup (NULL);
    is(res, NULL, "handles NULL input") or diag("got \"%s\"", res);
    free (res);

    res = strdup ("hello world");
    is_str(res, "hello world", "handles normal input") or diag("got \"%s\"", res);
    free (res);

    done_testing ();
    return exit_status ();
}

/* end of file */
