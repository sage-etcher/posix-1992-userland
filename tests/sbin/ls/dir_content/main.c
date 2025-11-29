
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


static int
strarr_diag (char **arr, size_t n)
{
    diag ("got:");
    for (; n > 0; n--, arr++)
    {
        diag ("\t\"%s\"", *arr);
    }

    return 0;
}

static int
str_in_strarr (char **haystack, size_t haystack_n, char *needle)
{
    for (; haystack_n > 0; haystack_n--, haystack++)
    {
        if (strcmp (*haystack, needle) == 0) { return 0; }
    }
    return 1;
}

static int
strarr_in_strarr (char **haystack, size_t haystack_n, char **needles, size_t needles_n)
{
    for (; needles_n > 0; needles_n--, needles++)
    {
        if (str_in_strarr (haystack, haystack_n, *needles) != 0) { return 1; }
    }
    return 0;
}

int
main(int argc, char **argv)
{
    size_t i = 0;
    size_t len = 0;
    const char *base[5] = { 
        "trash1",
        "trash2",
        "trash3",
        "trash4",
        "trash5",
    };
    const char *intended[5] = { 
        "..",
        ".",
        "a.txt",
        "b.txt",
        "c.txt",
    };
    char *buf[5] = { 0 };

    qsort(base, 3, sizeof(*base), (qsort_cb_t)strcmp);

    len = dir_content(NULL, 10, "testdir");
    is(len, 5, "handles NULL buffer") or diag("got %zu", len);

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 0, "testdir");
    is(len, 0, "handles zero length buffer: length") or diag("got %zu", len);
    is_mem(buf, base, sizeof (base), "handles null directory name: buffer untouched") or strarr_diag(buf, 5);

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 5, NULL);
    is(len, -1, "handles null directory name: length") or diag("got %zu", len);
    is_mem(buf, base, sizeof (base), "handles null directory name: buffer untouched") or strarr_diag(buf, 5);

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 5, "does not exist");
    is(len, -1, "handles non-existant directory: length") or diag("got %zu", len);
    is_mem(buf, base, sizeof (base), "handles null directory name: buffer untouched") or strarr_diag(buf, 5);

    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 5, "testdir");
    is(len, 5, "intended usage buffer non-null: length") or diag("got %zu", len);
    qsort(buf, len, sizeof(*buf), (qsort_cb_t)strcmp);
    ok(0 == strarr_cmp((const char **)buf, intended, 5), "intended usage buffer non-null: content") or strarr_diag(buf, 5);

    len = dir_content(NULL, 0, "testdir");
    is(len, 5, "intended usage buffer null: length") or diag("got %zu", len);
    
    memcpy (buf, base, sizeof (base));
    len = dir_content(buf, 3, "testdir");
    is(len, 3, "short buffer: length") or diag("got %zu", len);
    qsort(buf, len, sizeof(*buf), (qsort_cb_t)strcmp);
    ok(0 == strarr_in_strarr((char **)intended, 5, (char **)buf, 3), "short buffer: content") or strarr_diag(buf, 5);

    done_testing();
    return exit_status();
}

/* end of file */
