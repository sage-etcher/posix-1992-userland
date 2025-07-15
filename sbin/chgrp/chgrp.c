
#define _POSIX_C_SOURCE 1

#include "chgrp.h"

#include "basename.h"
#include "convert.h"
#include "err.h"
#include "iterdir.h"
#include "vstring.h"

#include <errno.h>
#include <grp.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CHGRP_USAGE "chgrp [-R] group file..."


int
get_gid_s (const char *group_in, gid_t *p_gid)
{
    struct group *grp = NULL;
    gid_t gid = 0;

    if ((group_in == NULL) || (p_gid == NULL))
    {
        return (errno = EINVAL);
    }

    /* first try to use _group_in_ as a group name */
    grp = getgrnam (group_in);
    if (grp != NULL) 
    { 
        *p_gid = grp->gr_gid; 
        return 0;
    }

    /* if that fails, try it as a group number */
    if (0 == atou_s (group_in, &gid, 10))
    {
        *p_gid = gid;
        return 0;
    }

    return errno;
}

int
chgrp (gid_t gid, const char *path, int recurse)
{
    int rc = 0;

    struct dirent *dp = NULL;
    char  *new_buf = NULL;
    size_t new_len = 0;

    /* change group */
    if (chown (path, -1, gid))
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
        
        rc |= chgrp (gid, new_buf, recurse);
    }
    free (new_buf);
    new_buf = NULL;
    new_len = 0;
    iterdir_done ();

    return rc;
}


int
chgrp_main (int argc, char **argv)
{
    int rc = 0;
    int c = 0;

    /* settings */
    int recurse = 0;
    char *group_in = NULL;
    char **files = NULL;
    size_t file_count = 0;
    gid_t gid = 0;

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
            ERR_USAGE (CHGRP_USAGE);
            return 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 2)
    {
        ERR_USAGE (CHGRP_USAGE);
        return 1;
    }
    group_in = argv[0];
    files = &argv[1];
    file_count = argc-1;

    /* get group id */
    if (get_gid_s (group_in, &gid))
    {
        ERR_INVALID_GROUP (group_in);
        return 1;
    }

    /* loop over files */
    for (; file_count > 0; file_count--, files++)
    {
        rc |= chgrp (gid, *files, recurse);
    }

    return rc;
}

/* end of file */
