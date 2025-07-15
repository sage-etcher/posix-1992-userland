
#include "basename.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

char *
basename (char *string, char *suffix)
{
    char *a = NULL;
    char *b = NULL;

    /* handle empty and null _string_ */
    if ((string == NULL) || !(*string)) { return string; }

    /* move _a_ to the end of _string_
     * iterate back until _a_ is either:
     *   at the base of string, or
     *   at a non-slash (/) character.
     * step _a_ forward by 1 character and place the NULL terminator. */
    for (a = string; *a; a++) { }
    for (a--; a > string && *a == '/'; a--) { }
    a[1] = '\0';

    /* if string is a single slash, return _string_ */
    if (0 == strcmp (string, "/")) { return string; }

    /* otherwise, store a pointer to the end of string */
    b = a + 1;

    /* and continue to iterate back until _a_ is either:
     *   at 1 character before the base of _string_, or
     *   at a slash (/) character.
     * step _a_ forward by 1 character and assign _a_ to be the new _string_ */
    for (a--; a >= string && *a != '/'; a--) { }
    string = a + 1;

    /* if no suffix is given, return _string_ as is */
    if (!suffix) { return string; }

    /* otherwise, step _b_ (end of _string_) back _suffix_.length elements.
     * if _b_ points before or at the start of _string_, return _string_ */
    b -= strlen (suffix);
    if (b <= string) { return string; }

    /* finally, compare _b_ and _suffix_, if they match, place a NULL 
     * terminator at _b_ and return. */
    if (0 == strcmp (b, suffix)) { *b = '\0'; }

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
