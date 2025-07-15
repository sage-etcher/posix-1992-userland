
#define _POSIX_C_SOURCE 1

#include "convert.h"

#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>



int 
atou_s (const char *a, unsigned *p_u, unsigned base)
{
    char c = 0;
    unsigned u = 0;
    const char *value_list = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *match = NULL;
    unsigned value = 0;

    if ((a == NULL) || (p_u == NULL))
    { 
        return (errno = EINVAL);
    }

    /* convert to unsigned */
    for (; *a; a++)
    {
        c = toupper (*a);
        match = strchr (value_list, c);
        value = match - value_list;

        if (value >= base)
        {
            return (errno = EINVAL);
        }

        u *= base;
        u += value;
    }

    *p_u = u;
    return 0;
}

/* end of file */
