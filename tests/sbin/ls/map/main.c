
#include <ctype.h>

#include "ls.h"
#include "tap.h"

#define ARRLEN(a) (sizeof (a) / sizeof (*(a)))


void
map_to_upper (void *cb_data, void *elem)
{
    *(char *)elem = toupper(*(char *)elem);
}

void
map_scale (void *cb_data, void *elem)
{
    *(int *)elem *= *(int *)cb_data;
}

int
main (int argc, char **argv)
{
    char text[] = "hello world!";
    map (text, ARRLEN (text), sizeof (*text), map_to_upper, NULL);
    is_str (text, "HELLO WORLD!", "intended with character array and no cb_data") or diag("got %.*s", ARRLEN(text), text);

    const int base[5] = { 0, 1, 2, 3, 4 };

    int iarr[5]     = { 0 };
    int expected[5] = { 0, 3, 6, 9, 12 };
    int scale_value = 3;

    memcpy (iarr, base, sizeof (base));
    map (iarr, ARRLEN (iarr), sizeof (*iarr), map_scale, &scale_value);
    is_mem(iarr, expected, sizeof (iarr), "intended with integar arrays and cb_data");

    map (NULL, ARRLEN (iarr), sizeof (*iarr), map_scale, &scale_value);
    pass ("handles NULL buffer");

    memcpy (iarr, base, sizeof (base));
    map (iarr, 0, sizeof (*iarr), map_scale, &scale_value);
    is_mem(iarr, base, sizeof (iarr), "handles zero array_count");

    memcpy (iarr, base, sizeof (base));
    map (iarr, ARRLEN (iarr), 0, map_scale, &scale_value);
    is_mem(iarr, base, sizeof (iarr), "handles zero elem_size");

    memcpy (iarr, base, sizeof (base));
    map (iarr, ARRLEN (iarr), sizeof (*iarr), NULL, &scale_value);
    is_mem(iarr, base, sizeof (iarr), "handles NULL callback");

    done_testing ();
    return exit_status ();
}

/* end of file */
