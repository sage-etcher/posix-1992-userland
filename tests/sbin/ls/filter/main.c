
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
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(buf, len, sizeof (*buf), filter_even, NULL);
    is(len, 3,    "callback_no_data: len == 3")    or diag("got %zu", len);
    is(buf[0], 0, "callback_no_data: buf[0] == 0") or diag("got %d", buf[0]);
    is(buf[1], 2, "callback_no_data: buf[1] == 2") or diag("got %d", buf[1]);
    is(buf[2], 4, "callback_no_data: buf[2] == 4") or diag("got %d", buf[2]);
}

void callback_with_data (void)
{
    int max_value = 2;
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(buf, len, sizeof (*buf), filter_less_than, &max_value);
    is(len, 2,    "callback_with_data: len == 2")    or diag("got %zu", len);
    is(buf[0], 0, "callback_with_data: buf[0] == 0") or diag("got %d", buf[0]);
    is(buf[1], 1, "callback_with_data: buf[1] == 1") or diag("got %d", buf[1]);
}

void null_buffer (void)
{
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(NULL, len, sizeof (*buf), filter_even, NULL);
    is(len, 0, "null_buffer: len == 0") or diag("got %zu", len);
}

void zero_length (void)
{
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(buf, 0, sizeof (*buf), filter_even, NULL);
    is(len, 0,    "zero_length: len == 0")    or diag("got %zu", len);
    is(buf[0], 0, "zero_length: buf[0] == 0") or diag("got %d", buf[0]);
    is(buf[1], 1, "zero_length: buf[1] == 1") or diag("got %d", buf[1]);
    is(buf[2], 2, "zero_length: buf[2] == 2") or diag("got %d", buf[2]);
    is(buf[3], 3, "zero_length: buf[3] == 3") or diag("got %d", buf[3]);
    is(buf[4], 4, "zero_length: buf[4] == 4") or diag("got %d", buf[4]);
}

void zero_elem_size (void)
{
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(buf, len, 0, filter_even, NULL);
    is(len, 0,    "zero_elem_size: len == 0")    or diag("got %zu", len);
    is(buf[0], 0, "zero_elem_size: buf[0] == 0") or diag("got %d", buf[0]);
    is(buf[1], 1, "zero_elem_size: buf[1] == 1") or diag("got %d", buf[1]);
    is(buf[2], 2, "zero_elem_size: buf[2] == 2") or diag("got %d", buf[2]);
    is(buf[3], 3, "zero_elem_size: buf[3] == 3") or diag("got %d", buf[3]);
    is(buf[4], 4, "zero_elem_size: buf[4] == 4") or diag("got %d", buf[4]);
}

void null_callback (void)
{
    int buf[5] = { 0, 1, 2, 3, 4 };
    size_t len = 5;

    len = filter(buf, len, sizeof (*buf), NULL, NULL);
    is(len, 0,    "null_callback: len == 0")    or diag("got %zu", len);
    is(buf[0], 0, "null_callback: buf[0] == 0") or diag("got %d", buf[0]);
    is(buf[1], 1, "null_callback: buf[1] == 1") or diag("got %d", buf[1]);
    is(buf[2], 2, "null_callback: buf[2] == 2") or diag("got %d", buf[2]);
    is(buf[3], 3, "null_callback: buf[3] == 3") or diag("got %d", buf[3]);
    is(buf[4], 4, "null_callback: buf[4] == 4") or diag("got %d", buf[4]);
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
