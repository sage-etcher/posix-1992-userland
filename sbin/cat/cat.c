
#include "cat.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>


static void
unbuffered_cat (FILE *fp)
{
    int c = 0;

    while (EOF != (c = getc (fp)))
    {
        (void)putc (c, stdout);
    }
}

#define BUF_SIZE 4096
static void 
buffered_cat (FILE *fp)
{
    static unsigned char s_buf[BUF_SIZE];
    unsigned long cnt = 0;
    
    do 
    {
        cnt = fread (s_buf, sizeof (*s_buf), BUF_SIZE, fp);
        (void)fwrite (s_buf, sizeof (*s_buf), cnt, stdout);
    }
    while (cnt == BUF_SIZE);
}

static int
cat_single (char *filename, int unbuffered)
{
    FILE *fp = NULL;

    if (0 == strcmp (filename, "-"))
    {
        /* use stdin */
        fp = stdin;
    }
    else 
    {
        /* open file */
        errno = 0;
        fp = fopen (filename, "rb");
        if (fp == NULL)
        {
            (void)fprintf (stderr, "cat: cannot access file %s: %s\n", filename, strerror (errno));
            return 1;
        }
    }

    if (unbuffered) { unbuffered_cat (fp); }
    else { buffered_cat (fp); }

    if (stdin == fp)
    {
        (void)fclose (fp);
    }
    fp = NULL;

    return 0;
}


int
cat_main (int argc, char **argv)
{
    int unbuffered = 0;
    char *filename = NULL;

    /* remove the cat filename */
    argc--;
    argv++;

    /* if -u (unbuffered) was specified, set the flag and pop the element */
    if ((argc > 0) && (0 == strcmp (*argv, "-u")))
    {
        unbuffered = 1;
        argc--;
        argv++;
    }

    filename = (argc > 0) ? *argv : "-";

    do 
    {
        cat_single (filename, unbuffered);

        argc--;
        argv++;
        filename = *argv;
    }
    while (argc > 0);

    return 0;
}
/* end of file */
