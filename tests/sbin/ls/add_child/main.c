
#include <stdio.h>
#include <string.h>

#include "ls.h"

int g_test_count = 0;
int g_exit_status = 0;

#define log(prefix,msg) \
    do { \
        printf("%s %d ", prefix, g_test_count); \
        if ((msg) != NULL) { \
            printf("- %s", msg); \
        } \
        printf("\n"); \
    }
#define pass(msg) log("ok", msg)

#define fail(msg) \
    do { \
        log("not ok", msg); \
        g_exit_status = 1; \
    } while (0)

#define skip(n,msg) \
    do { \
        log("skip %d..%d", msg, g_test_count+1, g_test_count+(n)); \
        g_test_count+=(n); \
    } while (0)

#define diag(msg, ...) vprintf ("# "msg, __VA_ARGS)

#define ok(cond,msg)  \
    do { \
        g_test_count++; \
        if (cond) { pass(msg) } \
        else { fail(msg) } \
    } while (0)

#define plan(n) printf ("%d..%d\n", g_test_count+1, g_test_count+(n))

#define is_eq(x,y,msg) ok(((x) == (y)), msg)
#define is_neq(x,y,msg) ok(((x) != (y)), msg)

#define is_streq(x,y,msg) ok(strcmp(x, y), msg)
#define is_strneq(x,y,msg) ok(!strcmp(x, y), msg)

#define exit_status() g_exit_status


int
main (int argc, char **argv)
{
    plan(5);

    ok(1, "always true");

    is_streq("/etc", "/etc", "identical constant strings");
    is_streq("/tmp", "/etc", "different constant strings");
    is_streq(NULL, NULL, "both null");
    is_streq("/etc", NULL, "2nd param null");
    is_streq(NULL, "/etc", "1st param null");

    return exit_status();
}

/* end of file */
