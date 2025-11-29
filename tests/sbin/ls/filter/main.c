
#include "tap.h"
#include "ls.h"

int
filter_even (void *cb_data, void *elem)
{
    return !(*(int *)elem & 1);
}


int
filter_less_than (void *cb_data, void *elem)
{
    return *(int *)elem < *(int *)cb_data;
}

void
callback_no_data (void)
{
    const int base[5]     = { 0, 1, 2, 3, 4 };
    const int expected[3] = { 0, 2, 4 };
    int buf[5] = { 0 };
    size_t len = 5;

    memcpy (buf, base, sizeof (buf));
    len = filter(buf, len, sizeof (*buf), filter_even, NULL);
    is(len, 3,               "intended without data: length") or diag("got %zu", len);
    is_mem(buf, expected, 3, "intended without data: results");
}

void callback_with_data (void)
{
    const int base[5]     = { 0, 1, 2, 3, 4 };
    const int expected[3] = { 0, 2, 4 };
    int buf[5] = { 0 };
    int max_value = 2;
    size_t len = 5;

    memcpy (buf, base, sizeof (buf));
    len = filter(buf, len, sizeof (*buf), filter_less_than, &max_value);
    is(len, 2,               "intended with data: length") or diag("got %zu", len);
    is_mem(buf, expected, 2, "intended with data: results");
}

void null_buffer (void)
{
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(NULL, len, sizeof (*buf), filter_even, NULL);
    is(len, 0, "handles null buffer: returns 0") or diag("got %zu", len);
}

void zero_length (void)
{
    const int base[5] = { 0, 1, 2, 3, 4 };
    int buf[5] = { 5 };
    size_t len = 5;

    memcpy (buf, base, sizeof (buf));
    len = filter(buf, 0, sizeof (*buf), filter_even, NULL);
    is(len, 0,                      "handles zero length: returns 0")    or diag("got %zu", len);
    is_mem(buf, base, sizeof (buf), "handles zero length: buffer untouched");
}

void zero_elem_size (void)
{
    const int base[5] = { 0, 1, 2, 3, 4 };
    int buf[5] = { 5 };
    size_t len = 5;

    memcpy (buf, base, sizeof (buf));
    len = filter(buf, len, 0, filter_even, NULL);
    is(len, 0,                      "handles zero elem_size: returns 0") or diag("got %zu", len);
    is_mem(buf, base, sizeof (buf), "handles zero elem_size: buffer untouched");
}

void null_callback (void)
{
    const int base[5] = { 0, 1, 2, 3, 4 };
    int buf[5] = { 5 };
    size_t len = 5;

    memcpy (buf, base, sizeof (buf));
    len = filter(buf, len, sizeof (*buf), NULL, NULL);
    is(len, 0,                      "handles null callback: returns 0") or diag("got %zu", len);
    is_mem(buf, base, sizeof (buf), "handles null callback: buffer untouched");
}


int
main (int argc, char **argv)
{
    callback_no_data ();
    callback_with_data ();

    null_buffer (); 
    zero_length ();
    zero_elem_size ();
    null_callback ();

    done_testing();
    return exit_status();
}

/* end of file */
