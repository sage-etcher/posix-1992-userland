
#include <stdio.h> /* legacy posix getopt provider */
#include <unistd.h>

enum {
    COLUMN_MODE = 1 << 1,
    SUFFIXES    = 1 << 2,
    RECURSIVE   = 1 << 3,
    HIDDEN      = 1 << 4,
    FILE_STATUS = 1 << 5,
    NO_DIRECTORY= 1 << 6,
    INODE_MODE  = 1 << 7,
    LONG_MODE   = 1 << 8,
    PRINTABLE   = 1 << 9,
    SORT_REVERSE= 1 << 10,
    SORT_TIME   = 1 << 11,
    FILE_ACCESS = 1 << 12,
    SINGLE_MODE = 1 << 13,

    PRINT_MODES = COLUMN_MODE | LONG_MODE | INODE_MODE | SINGLE_MODE,
    TIME_MODES  = FILE_STATUS | FILE_ACCESS,
};

#define ENABLE_BFLAG(var, flag)     ((var) |= (flag))
#define DISABLE_BFLAG(var, flag)    ((var) &= ~(flag))
#define IS_BFLAG(var, flag)         ((var) & (flag))


int
get_config (int argc, char **argv)
{
    int config = SINGLE_MODE | FILE_STATUS;
    int c = 0;

    while (-1 != (c = getopt (argc, argv, "CFRacdilqrtu1")))
    {
        switch (c)
        {
        case 'C':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, COLUMN_MODE);
            break;

        case 'F':
            ENABLE_BFLAG (config, SUFFIXES);
            break;

        case 'R':
            ENABLE_BFLAG (config, RECURSIVE);
            break;

        case 'a':
            ENABLE_BFLAG (config, HIDDEN);
            break;

        case 'c':
            DISABLE_BFLAG (config, TIME_MODES);
            ENABLE_BFLAG (config, FILE_STATUS);
            break;

        case 'd':
            ENABLE_BFLAG (config, NO_DIRECTORY);
            break;

        case 'i':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, INODE_MODE);
            break;

        case 'l':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, LONG_MODE);
            break;

        case 'r':
            ENABLE_BFLAG (config, SORT_REVERSE);
            break;

        case 't':
            ENABLE_BFLAG (config, SORT_TIME);
            break;

        case 'u':
            ENABLE_BFLAG (config, FILE_ACCESS);
            break;

        case '1':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, SINGLE_MODE);
            break;

        case '?':
        default:
            (void)printf ("usage: ls [-CFRacdilqrtu1][file...]\n");
            goto exit;
        }
    }

exit:
    return config;
}

int
ls_main (int argc, char **argv)
{
    int config = get_config (argc, argv);
    
    printf ("%016X\n", config);

    return 0;

}

int
main (int argc, char **argv)
{
    return ls_main (argc, argv);
}

/* end of file */
