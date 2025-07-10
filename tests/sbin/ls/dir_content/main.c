
#include <string.h>
#include <stdlib.h>

#include "tap.h"
#include "ls.h"

#define MIN(x,y) ((x) < (y) ? (x) : (y))

typedef int(*qsort_cb_t)(const void *a, const void *b);

static int
strarr_cmp (const char **a, const char **b, size_t n)
{
    if ((a == NULL) || (b == NULL)) { return 0; }

    for (; n > 0; n--, a++, b++)
    {
        if (strcmp (*a, *b) == 0) { continue; }
        return 1;
    }

    return 0;
}


int
main(int argc, char **argv)
{
    size_t i = 0;
    size_t len = 0;
    const char *base[3] = { 
        "trash1",
        "trash2",
        "trash3",
    };
    const char *intended[3] = { 
        "a.txt",
        "b.txt",
        "c.txt",
    };
    char *buf[3] = { 0 };

    qsort(base, 3, sizeof(*base), (qsort_cb_t)strcmp);

    len = dir_content(NULL, 10, "testdir");
    is(len, 3, "handles NULL buffer");

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 0, "testdir");
    is(len, 0, "handles zero length buffer: length");
    ok(memcmp(buf, base, sizeof (base)), "handles zero length buffer: buffer untouched");

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 3, NULL);
    is(len, 0, "handles null directory name: length");
    ok(memcmp(buf, base, sizeof (base)), "handles null directory name: buffer untouched");

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 3, "does not exist");
    is(len, 0, "handles non-existant directory: length");
    ok(memcmp(buf, base, sizeof (base)), "handles non-existant directory: buffer untouched");

    len = dir_content(buf, 3, "testdir");
    is(len, 3, "intended usage buffer non-null: length");
    qsort(buf, len, sizeof(*buf), (qsort_cb_t)strcmp);
    ok(strarr_cmp((const char **)buf, intended, 3), "intended usage buffer non-null: content");

    len = dir_content(NULL, 0, "testdir");
    is(len, 3, "intended usage buffer null: length");

    done_testing();
    return exit_status();
}

/* end of file */
