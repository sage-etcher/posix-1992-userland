
#define _POSIX_C_SOURCE 1

#include "vstring.h"

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int
vstrncat_auto (char *buf, size_t n, size_t i, va_list args)
{
    char *iter = buf;
    size_t count = 0;

    const char *arg = NULL;
    size_t arg_len = 0;

    for (; i > 0; i--)
    {
        arg = va_arg (args, const char *);
        arg_len = strlen (arg);
        
        count += arg_len;
        
        if (buf == NULL) { continue; }
        strncat (iter, arg, n);
        iter  += arg_len;
    }

    return count;
}

size_t 
strncat_auto (char *buf, size_t n, size_t i, ...)
{
    size_t rc = 0;
    va_list args;
    va_start (args, i);
    rc = vstrncat_auto (buf, n, i, args);
    va_end (args);
    return rc;
}

size_t 
strcat_auto (char *buf, size_t i, ...)
{
    size_t rc = 0;
    va_list args;
    va_start (args, i);
    rc = vstrncat_auto (buf, -1, i, args);
    va_end (args);
    return rc;
}

char *
strdup (const char *src)
{
    size_t size = 0;
    char *dst = NULL;

    if (src == NULL) { return NULL; }

    /* allocate dst */
    size = strlen (src);
    dst = malloc (size + 1);
    if (dst == NULL) { return NULL; }

    /* copy data over */
    (void)strcpy (dst, src);

    return dst;
}


/* end of file */
