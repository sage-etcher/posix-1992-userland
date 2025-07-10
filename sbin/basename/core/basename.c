
#include "basename.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>


int
basename_main (int argc, char **argv)
{
    int all_slashes = 0;
    char *iter = NULL;

    char *string = NULL;
    char *suffix = NULL;

    if (argc < 2) 
    { 
        printf ("usage: basename string [suffix]\n"); 
        return 1;
    };
    string = argv[1];

    if (argc >= 3)
    {
        suffix = argv[2];
    }

    /* (1) if _string_ is "//" it is implementation defined if steps 2-5 are 
     *     skipped. */
    if (0 == strcmp (string, "//")) 
    {
        puts ("/");
        return 0;
    }

    /* (2) if _string_ is all '/' characters, _string_ should be set to a 
     *     single "/", skip 3-5. */
    for (iter = string; *iter && (all_slashes = (*iter == '/')); iter++) { }
    if (all_slashes)
    {
        puts ("/");
        return 0;
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
        puts (string);
        return 0;
    }

    iter = string + strlen(string) - strlen(suffix);
    if (0 == strcmp (iter, suffix))
    {
        *iter = '\0';
    }

    puts (string);

    return 0;
}
/* end of file */
