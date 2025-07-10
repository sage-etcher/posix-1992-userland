
#include "basename.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *
basename (char *string, char *suffix)
{
    int all_slashes = 0;
    char *iter = NULL;

    /* (1) if _string_ is "//" it is implementation defined if steps 2-5 are 
     *     skipped. */

    /* (2) if _string_ is all '/' characters, _string_ should be set to a 
     *     single "/", skip 3-5. */
    for (iter = string; *iter && (all_slashes = (*iter == '/')); iter++) { }
    if (all_slashes)
    {
        string[0] = '/';
        string[1] = '\0';
        return string;
    }

    /* (3) remove trailing '/' characters from _string_. */
    for (iter = string; *iter; iter++) { }
    for (iter--; (iter >= string) && (*iter == '/'); iter--) { }
    iter[1] = '\0';

    /* (4) remove all characters up to and including the first '/' character. */
    for (; (iter >= string) && (*iter != '/'); iter--) { }
    string = iter+1;

    /* (5) if _suffix_ operand is present, and isn't identical to the remaining 
     *     _string_, but does match the suffix of _string_, remove the suffix, 
     *     otherwise, do nothing. 
     *     note: a missing suffix is NOT an error condition. */
    if ((suffix == NULL) ||
        (strlen (string) <= strlen (suffix)) ||
        (0 == strcmp (string, suffix)))
    {
        return string;
    }

    iter = string + strlen(string) - strlen(suffix);
    if (0 == strcmp (iter, suffix))
    {
        *iter = '\0';
    }

    return string;
}

int
basename_main (int argc, char **argv)
{
    char *string = NULL;
    char *suffix = NULL;

    if (argc < 2) 
    { 
        (void)fprintf (stderr, "usage: basename string [suffix]\n"); 
        return 1;
    };
    string = argv[1];

    if (argc >= 3)
    {
        suffix = argv[2];
    }

    printf ("%s\n", basename (string, suffix));

    return 0;
}
/* end of file */
