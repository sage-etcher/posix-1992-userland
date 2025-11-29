
#include <limits.h>

#include "tap.h"
#include "ls.h"

int
main (int argc, char **argv)
{
    size_t res = 0;

    res = lu_len (0);
    is(res, 1, "zero input");

    res = lu_len (1);
    is(res, 1, "1 digit");

    res = lu_len (21);
    is(res, 2, "2 digit");

    res = lu_len (215);
    is(res, 3, "3 digit");

    res = lu_len (1075);
    is(res, 4, "4 digit");

    res = lu_len (4294967295);
    is(res, 10, "max 32bit unsigned");

    if (0xffffffffffffffff > ULONG_MAX)
    {
        skip (1, "platform's unsigned long is not 64bits");
    }
    res = lu_len (18446744073709551615);
    is(res, 20, "max 64bit unsigned");

    done_testing ();
    return exit_status ();
}

/* end of file */
