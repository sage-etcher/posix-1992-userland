
#include "functional.h"

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>


int
filter_s (void *tarr, size_t len, size_t tsize, filter_cb_t cb, void *data, 
          size_t *p_new_len)
{
    /* {{{ */
    unsigned char *src = tarr;
    unsigned char *dst = tarr;
    size_t new_len = 0;

    if ((tarr == NULL) || (!len) || (!tsize) || (cb == NULL) || 
        (p_new_len == NULL))
    {
        return (errno = EINVAL);
    }

    for (; len > 0; len--)
    {
        if (cb (data, src))
        {
            (void)memcpy (dst, src, tsize);
            dst += tsize;
            new_len++;
        }
        src += tsize;
    }

    *p_new_len = new_len;
    return 0;
    /* }}} */
}

int
map_s (void *tarr, size_t len, size_t tsize, map_cb_t cb, void *data)
{
    unsigned char *iter = tarr;

    if ((tarr == NULL) || (!len) || (!tsize) || (cb == NULL))
    {
        return (errno = EINVAL);
    }

    for (; len > 0; len--)
    {
        cb (data, iter);
        iter += tsize;
    }

    return 0;
    /* }}} */
}

/* end of file */
