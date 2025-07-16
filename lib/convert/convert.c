
#define _POSIX_C_SOURCE 1

#include "convert.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#define CH_DIGITS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"


int
atolu_s (const char *a, long unsigned *p_lu, short unsigned base)
{
    /* {{{ */
    char c = 0;
    unsigned lu = 0;
    const char *const digits = CH_DIGITS;
    const char *match = NULL;
    unsigned value = 0;

    if ((a == NULL) || (p_lu == NULL))
    { 
        return (errno = EINVAL);
    }

    if (base >= strlen (digits))
    {
        return (errno = ERANGE);
    }

    /* convert to unsigned */
    for (; *a; a++)
    {
        c = toupper (*a);
        match = strchr (digits, c);
        value = match - digits;

        if (value >= base)
        {
            return (errno = EINVAL);
        }

        lu *= base;
        lu += value;
    }

    *p_lu = lu;
    return 0;
    /* }}} */
}

int 
atou_s (const char *a, unsigned *p_u, short unsigned base)
{
    /* {{{ */
    long unsigned lu = 0;
    int rc = 0;

    if (p_u == NULL)
    {
        return (errno = EINVAL);
    }

    rc = atolu_s (a, &lu, base);
    if (rc != 0) { return rc; }
    if (lu > UINT_MAX)
    {
        return (errno != ERANGE);
    }

    *p_u = (unsigned)lu;
    return 0;
    /* }}} */
}

size_t
lulen (long unsigned lu, short unsigned base)
{
    /* {{{ */
    size_t n = 0;
    
    if (lu == 0) { return 1; }

    for (n = 0; lu > 0; lu /= base, n++) { }

    return n;
    /* }}} */
}

int
lutoa_s (char *buf, size_t n, long unsigned ul, short unsigned base)
{
    /* {{{ */
    const char *const digits = CH_DIGITS;

    if ((buf == NULL) || (n == 0))
    {
        return (errno = EINVAL);
    }

    if (base >= strlen (digits))
    {
        return (errno = ERANGE);
    }

    buf[--n] = '\0';
    for (; n > 0; ul /= base)
    {
        buf[--n] = digits[ul % base];
    }

    return 0;
    /* }}} */
}

/* vim: fdm=marker
 * end of file */
