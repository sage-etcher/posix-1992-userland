
#include <limits.h>
#include <stdlib.h>

#include "tap.h"
#include "ls.h"

int
main (int argc, char **argv)
{
    char *buf = snprintlu_dup (0);
    is_str(buf, "0", "handles zero") or diag("got %s", buf);
    free (buf);

    buf = snprintlu_dup (1);
    is_str(buf, "1", "1 digit") or diag("got %s", buf);
    free (buf);

    buf = snprintlu_dup (10);
    is_str(buf, "10", "2 digit") or diag("got %s", buf);
    free (buf);

    buf = snprintlu_dup (593);
    is_str(buf, "593", "3 digit") or diag("got %s", buf);
    free (buf);

    buf = snprintlu_dup (1107);
    is_str(buf, "1107", "4 digit") or diag("got %s", buf);
    free (buf);
    
    buf = snprintlu_dup (0xffffffff);
    is_str(buf, "4294967295", "max 32bit unsigned") or diag("got %s", buf);
    free (buf);
    
    if (0xffffffffffffffff > ULONG_MAX)
    {
        skip(1, "no support for 64bit unsigned longs");
    }
    buf = snprintlu_dup (0xffffffffffffffff);
    is_str(buf, "18446744073709551615", "max 64bit unsigned") or diag("got %s", buf);
    free (buf);

    done_testing ();
    return exit_status ();
}

/* end of file */
