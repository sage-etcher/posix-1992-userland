
#include "chmod.h"

#include "basename.h"
#include "convert.h"
#include "err.h"
#include "iterdir.h"
#include "vstring.h"

#include <errno.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define CHMOD_USAGE "chmod [-R] mode file..."


int
get_mode_s (const char *mode_in, mode_t *p_mode)
{
    return 0;
}

int
chmod_handler (mode_t mode, const char *path, int recurse)
{
    int rc = 0;

    struct dirent *dp = NULL;
    char  *new_buf = NULL;
    size_t new_len = 0;

    /* change group */
    if (chmod (path, mode))
    {
        rc = 1;
        ERR_ACCESS (path, errno);
    }

    /* and do nothing more if non recursive */
    if (!recurse) { return rc; }

    /* but if it is! loop through it, recurively */
    if (!is_dir (path)) { return rc; }
    for (dp = iterdir (path); dp != NULL; dp = iterdir (NULL))
    {
        /* dont traverse special directories */
        if (is_special_dir (dp->d_name)) { continue; }

        new_len = strcat_auto (NULL, 3, path, "/", dp->d_name);
        new_buf = realloc (new_buf, new_len+1);
        *new_buf = '\0';
        (void)strcat_auto (new_buf, 3, path, "/", dp->d_name);
        
        rc |= chmod_handler (mode, new_buf, recurse);
    }
    free (new_buf);
    new_buf = NULL;
    new_len = 0;
    iterdir_done ();

    return rc;
}


int
chmod_main (int argc, char **argv)
{
    int rc = 0;
    int c = 0;

    /* settings */
    int recurse = 0;
    char *mode_in = NULL;
    char **files = NULL;
    size_t file_count = 0;
    mode_t mode = 0;

    /* configure err */
    g_err_prefix = basename (argv[0], NULL);

    /* get options */
    while ((c = getopt (argc, argv, "R")) != -1)
    {
        switch (c)
        {
        case 'R':
            recurse = 1;
            break;

        default:
            ERR_USAGE (CHMOD_USAGE);
            return 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 2)
    {
        ERR_USAGE (CHMOD_USAGE);
        return 1;
    }
    mode_in = argv[0];
    files = &argv[1];
    file_count = argc-1;

    /* get group id */
    if (get_mode_s (mode_in, &mode))
    {
        ERR_INVALID_GROUP (mode_in);
        return 1;
    }

    /* loop over files */
    for (; file_count > 0; file_count--, files++)
    {
        rc |= chmod_handler (mode, *files, recurse);
    }

    return rc;
}

/* end of file */
