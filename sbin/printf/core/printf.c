
#include "printf.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define ABS_UNSAFE(x) ((x) < 0 ? -(x) : (x))
#define MAX_UNSAFE(x,y) ((x) < (y) ? (y) : (x))

/* 
 * regex: `(\\[\\abfnrtv]|%[-+ #0]*\d*(\.\d*)?[diouxXfeEgGcs%])`
 *
 * format specification:
 *   " "       <blank>+
 *   triangle  <space>{1}
 *   \\        backslash
 *   \a        <alert>            alert the user
 *   \b        <backspace>        cursor to the left 1, no wrap
 *   \f        <form-feed>        cursor to next page
 *   \n        <newline>          cursor to start of next line
 *   \r        <carriage-return>  cursor to start of current line
 *   \t        <tab>              cursor to next tab pos, undefined if none available
 *   \v        <vertical tab>     cursor to next vertical tab pos, undefined if none available
 *
 *   <flags>    zero or more flags that modify the conversion
 *                `-`        left-justified [dominant `-`]
 *                `+`        signed: print sign ('+' or '-') [dominant `+`]
 *                `<space>`  signed: dont print sign, replace with <space> prefix [submissive `+`] 
 *                `#`        use alternate form: 
 *                             `[cdius]`  undefined
 *                             `o`        increase precision to force first digit t obe zero
 *                             `[xX]`     non-zero result have "0x" or "0X" prefix
 *                             `[eEfgG]`  result always has radix character, additionlly, 
 *                                          `[gG]` prints trailing zeroes aswel
 *                `0`        for `[diouxXeEfgG]`: leading zeros (prefixed by base/sign). [submissive `-`]
 *                             for `[diouxX]` if precision is specified, ignore `0`. 
 *
 *   <field width>    `\d*` specifying the minimum field width
 *
 *   <precision>    form: `.(\d*)`, a null digit string is treated as zero
 *                    `[doiuxX]`  max # of digits, padded with zeros (0).
 *                    `[ef]`      max # of digits to the left of the radix.
 *                    `g`         max # of significant digits.
 *                    `s`         max # of bytes to be written
 *
 *   <conversion characters>
 *          `[di]`  signed decimal
 *          `o`     unsigned octal
 *          `u`     unsigned decimal
 *          `[xX]`  unsigned hex using respecitive case
 *          `f`     floating point, LC_NUMERIC for radix character, if 0, dont print radix.
 *          `[eE]`  floating point exponential `[-]?d.ddde[+-]dd`. when precision is missing, 
 *                    use 6 digits after radix
 *          `[gG]`  floating point number, use `e` if exponent is is less than -4 or greater 
 *                    than or equal to the precision, otherwise use `f`.
 *          `c`     unsigned character
 *          `s`     string
 *          `%`     write a '%' character with no conversion.
 */


/* regex: `(\\[\\abfnrtv]|%[-+ #0]*\d*(\.\d*)?[diouxXfeEgGcs%])` */

enum {
    JUSTIFY_LEFT = 1 << 0,
    SHOW_SIGN    = 1 << 1,
    SPACE_SIGN   = 1 << 2,
    ALTERNATE    = 1 << 3,
    PAD_ZERO     = 1 << 4
};

typedef int (*case_cb_t)(int);

#define RERR_S "fprintf: cannot convert %s from string->%s: %s\n"
#define RERR_I "fprintf: cannot convert %i from integer->%s: %s\n"
#define RERR_U "fprintf: cannot convert %u from unsigned->%s: %s\n"


static int
ret_errno (const char *fmt, const char *dst_type, int errnum)
{
    (void)fprintf (stderr, fmt, dst_type, strerror (errnum));
    return (errno = errnum);
}

static int
my_atoi (char *a, int *p_i)
{
    int i = 0;
    int s = 1;

    if ((a == NULL) || (p_i == NULL))
    {
        return (errno = EINVAL);
    }

    switch (*a)
    {
    case '-':
        s = -1;
    case '+':
        a++;
        break;
    }

    for (; *a && isdigit (*a); a++)
    {
        if (INT_MAX / 10 - (*a - '0') < i)
        {
            return (errno = ERANGE);
        }
        i *= 10;
        i += *a - '0';
    }

    i *= s;

    *p_i = i;
    return 0;
}

static unsigned
my_atou (char *a, unsigned *p_u)
{
    unsigned u = 0;
    
    if ((a == NULL) || (p_u == NULL))
    {
        return (errno = EINVAL);
    }

    for (; *a && isdigit (*a); a++)
    {
        if (UINT_MAX / 10 - (*a - '0') < u)
        {
            return (errno = ERANGE);
        }
        u *= 10;
        u += *a - '0';
    }

    *p_u = u;
    return 0;
}

static int
u_base_len (unsigned u, unsigned base)
{
    int c = 1;

    for (; u > 0; u /= base, c++);

    return c;
}

static inline int
handle_decimal (char *arg, int flags, int min_width, int precision)
{
    const unsigned BASE = 10;

    int rc = 0;

    int num = 0;
    unsigned unum = 0;
    int required_unum_len = 0;
    int real_unum_len = 0;
    int sign_len = 0;
    int required_len = 0;
    int leading_zeros = 0;

    int padding = 0;

    /* converstion */
    rc = my_atoi (arg, &num);
    if (rc != 0)
    {
        return ret_errno (RERR_S, "integer", rc);
    }

    /* get i length */
    if (INT_MIN == num)
    {
        return ret_errno (RERR_I, "unsigned", ERANGE);
    }
    unum = ABS_UNSAFE (num);
    required_unum_len = u_base_len (unum, BASE);
    real_unum_len = MAX_UNSAFE (required_unum_len, precision);
    sign_len = flags & (SHOW_SIGN | SPACE_SIGN) ? 1 : 0;
    required_len = real_unum_len + sign_len;
    leading_zeros = real_unum_len - required_unum_len;
    padding = min_width - required_len;

    if (!(flags & JUSTIFY_LEFT))
    {
        for (; padding > 0; padding--)
        {
            putchar (' ');
        }
    }

    /* sign */
    if (flags & SHOW_SIGN)
    {
        putchar (num < 0 ? '-' : '+');
    }
    else if (flags & SPACE_SIGN)
    {
        putchar (' ');
    }

    /* padding zeros */
    if ((flags & PAD_ZERO) && !(flags & JUSTIFY_LEFT))
    {
        for (; leading_zeros > 0; leading_zeros--)
        {
            putchar ('0');
        }
    }

    /* right padding */
    if (flags & JUSTIFY_LEFT)
    {
        for (; padding > 0; padding--)
        {
            putchar (' ');
        }
    }

/* 
 * printf '%10d' -100
 * "       -100"
 *
 * printf '%010d' -100
 * "-000000100"
 *
 * printf '%-10d' -100
 * "-100      "
 *
 * printf '%-010d' -100
 * "-100      "
 *
 * */


    

    



    

    
    
}

static inline void 
handle_unsigned (char *arg, int flags, int min_width, int precision)
{
}

static inline void 
handle_octal (char *arg, int flags, int min_width, int precision)
{
}

static inline void 
handle_hex (char *arg, int flags, int min_width, int precision, case_cb_t c)
{
}

static inline void 
handle_float (char *arg, int flags, int min_width, int precision)
{
}

static inline void 
handle_exponential (char *arg, int flags, int min_width, int precision, case_cb_t c)
{
}

static inline void 
handle_graphic (char *arg, int flags, int min_width, int precision, case_cb_t c)
{
}

static inline void 
handle_char (char *arg, int flags, int min_width, int precision)
{
}

static inline void 
handle_string (char *arg, int flags, int min_width, int precision)
{
    for (; *arg; arg++)
    {
        putchar (*arg);
    }
}


void
format (char *fmt, char **arr, size_t n)
{
    int precision = 0;
    int min_width = 0;
    int flags = 0;

    for (; *fmt; fmt++)
    {
        if ('\\' == *fmt)
        {
            fmt++;
            switch (*fmt)
            {
            case '\\': putchar ('\\'); break;
            case 'a':  putchar ('\a'); break;
            case 'b':  putchar ('\b'); break;
            case 'f':  putchar ('\f'); break;
            case 'n':  putchar ('\n'); break;
            case 'r':  putchar ('\r'); break;
            case 't':  putchar ('\t'); break;
            case 'v':  putchar ('\v'); break;
            default:   fprintf (stderr, "printf: unknown escape character \\%c\n", *fmt);
            }
        }
        else if ('%' == *fmt)
        {
            min_width = 0;
            precision = 0;
            flags = 0;

            fmt++;
            
            /* get flags */
            for (; *fmt; fmt++)
            {
                switch (*fmt)
                {
                case '-': flags |= JUSTIFY_LEFT; break;
                case '+': flags |= SHOW_SIGN; break;
                case ' ': flags |= SPACE_SIGN; break;
                case '#': flags |= ALTERNATE; break;
                case '0': flags |= PAD_ZERO; break;
                default: goto field_width;
                }
            }

            /* get field width */
        field_width:
            for (; *fmt && isdigit (*fmt); fmt++)
            {
                min_width *= 10;
                min_width += *fmt - '0';
            }

            /* get precision */
            if (*fmt == '.')
            {
                fmt++;
                for (; *fmt && isdigit (*fmt); fmt++)
                {
                    precision *= 10;
                    precision += *fmt - '0';
                }
            }

            /* conversion character */
            switch (*fmt)
            {
            case 'd': case 'i': handle_decimal (*arr, flags, min_width, precision); break;
            case 'o': handle_octal (*arr, flags, min_width, precision); break;
            case 'u': handle_unsigned (*arr, flags, min_width, precision); break;
            case 'x': handle_hex (*arr, flags, min_width, precision, tolower); break;
            case 'X': handle_hex (*arr, flags, min_width, precision, toupper); break;
            case 'f': handle_float (*arr, flags, min_width, precision); break;
            case 'e': handle_exponential (*arr, flags, min_width, precision, tolower); break;
            case 'E': handle_exponential (*arr, flags, min_width, precision, toupper); break;
            case 'g': handle_graphic (*arr, flags, min_width, precision, tolower); break;
            case 'G': handle_graphic (*arr, flags, min_width, precision, toupper); break;
            case 's': handle_string (*arr, flags, min_width, precision); break;
            case '%': putchar ('%'); break;
            default:  fprintf (stderr, "printf: unkown conversion character %%%c\n", *fmt); break;
            }
            arr++;
            n--;

        }
        else
        {
            putchar (*fmt); 
        }
    }
}


int
printf_main (int argc, char **argv)
{
    format (argv[1]);
    return printf ("printf: hello world\n");
}

/* end of file */
