
#define _POSIX_C_SOURCE 1

#include "err.h"

#include <stdarg.h>
#include <stdio.h>

const char *g_err_prefix = NULL;

void
verr_prefix (const char *prefix, const char *fmt, va_list args) 
{
    fprintf (stderr, "%s: ", prefix);
    vfprintf (stderr, fmt, args);
    fputc ('\n', stderr);
}

void
err_prefix (const char *prefix, const char *fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    verr_prefix (prefix, fmt, args);
    va_end (args);
}

/* end of file */
