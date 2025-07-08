
#include <limits.h>

#include "ls.h"
#include "tap.h"
    
void
null_buffer (void)
{
    size_t len = snprintlu (NULL, len, 100);

    is(len, 4, "handles null buffer: return required length") or diag("got %zu", len);
}

void
zero_size (void)
{
    const char base[6] = "01234";
    char buf[6] = { 0 };
    size_t len = 0;

    memcpy (buf, base, sizeof (base));
    len = snprintlu (buf, 0, 100);

    is(len, 0,        "handles zero buffer size: returns 0") or diag("got %zu", len);
    is_str(buf, base, "handles zero buffer size: buffer untouched") or diag("got \"%.*s\"", 6, buf);
}

void
get_required_size (void)
{
    size_t len = 0;

    len = snprintlu (NULL, 0, 100);
    is(len, 5, "required size: 3 digit") or diag("got %zu", len);

    len = snprintlu (NULL, 0, 0);
    is(len, 2, "required size: zero input") or diag("got %zu", len);
    
    len = snprintlu (NULL, 0, 0xffffffff);
    is(len, 11, "required size: max 32bit unsigned") or diag("got %zu", len);

    if (0xffffffffffffffff > ULONG_MAX)
    {
        skip (1, "platform's unsigned long is not 64bits");
    }
    len = snprintlu (NULL, 0, 0xffffffffffffffff);
    is(len, 21, "required size: max 64bit unsigned") or diag("got %zu", len);
}

void
fill_buffer (void)
{
    char buf[21] = { 0 };

    (void)snprintlu (buf, 21, 0);
    is_str(buf, "0", "write: zero") or diag("got \"%21s\"", buf);

    (void)snprintlu (buf, 21, 137);
    is_str(buf, "137", "write: 3 digits") or diag("got \"%21s\"", buf);

    (void)snprintlu (buf, 21, 0xffffffff);
    is_str(buf, "4294967295", "write: max 32bit unsigned") or diag("got \"%21s\"", buf);

    if (0xffffffffffffffff > ULONG_MAX)
    {
        skip (1, "platform's unsigned long is not 64bits");
    }
    (void)snprintlu (buf, 21, 0xffffffffffffffff);
    is_str(buf, "18446744073709551615", "write: max 64bit unsigned") or diag("got \"%21s\"", buf);
}

void
buffer_size_too_small (void)
{
    char buf[21] = { 0 };
    size_t len = snprintlu (buf, 3, 1258);
    is(len, 3,        "buffer too small: return # of character written") or diag("got %zu", len);
    is_str(buf, "12", "buffer too small: write first n values") or diag("got \"%21s\"", buf);
}

int
main (int argc, char **argv)
{
    null_buffer ();
    zero_size ();

    get_required_size ();
    fill_buffer ();
    buffer_size_too_small ();

    done_testing ();
    return exit_status ();
}

/* end of file */
