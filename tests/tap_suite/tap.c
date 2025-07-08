
#include "tap.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int s_test_total   = 0;
static int s_test_count   = 0;
static int s_failed_count = 0;
static int s_skip_count   = 0;

static const char *s_last_test_name = NULL;
static int s_last_test_status = 0;

#define MIN(x,y) ((x) < (y) ? (x) : (y))

static void vnote (const char *fmt, va_list args);


void
plan (int number_of_tests)
{
    printf ("1..%d\n", number_of_tests);
    s_test_total = number_of_tests;
}

void
done_testing (void)
{
    plan (s_test_count);
}

int
ok (int condition, const char *test_name)
{
    s_test_count++;
    if (s_skip_count != 0)
    {
        s_skip_count--;
        return 1;
    }
    s_last_test_status = condition;
    s_last_test_name   = test_name;
    if (!condition)
    {
        s_failed_count++;
        printf("not ");
    }
    printf("ok %d", s_test_count);
    if (test_name != NULL)
    {
        printf(" - %s", test_name);
    }
    printf("\n");
    return condition;
}

int
pass (const char *test_name)
{
    return ok(1, test_name);
}

int
fail (const char *test_name)
{
    return ok(0, test_name);
}

void
skip (int number, const char *reason)
{
    s_skip_count = number;
    printf("skip %d..%d\n", s_test_count+1, s_test_count+number);
}

static void
vnote (const char *fmt, va_list args) 
{
    printf ("# ");
    vprintf (fmt, args);
    printf ("\n");
}

int
diag (const char *diagnostic_message, ...)
{
    va_list args;
    va_start (args, diagnostic_message);

    note ("\tFailed test '%s'", s_last_test_name);
    vnote (diagnostic_message, args);

    va_end (args);
    return 0;
}

int
note (const char *diagnostic_message, ...)
{
    va_list args;
    va_start (args, diagnostic_message);

    vnote (diagnostic_message, args);

    va_end (args);
    return 0;
}

int
exit_status (void)
{
    if ((s_test_total == s_test_count) && (s_failed_count == 0)) return 0;
    if ((s_test_total != s_test_count) && (s_failed_count == 0)) return 255;
    return MIN (s_failed_count, 254);
}

void
bail_out (const char *reason)
{
    fail (reason);
    exit (255);
}

int
strcmp_null_safe (const char *a, const char *b)
{
    if (a == NULL) return 1;
    if (b == NULL) return -1;
    return strcmp (a, b);
}

int
memcmp_null_safe (const void *a, const void *b, size_t n)
{
    if (a == NULL) return 1;
    if (b == NULL) return -1;
    return memcmp (a, b, n);
}


/* end of file */
