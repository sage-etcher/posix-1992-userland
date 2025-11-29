
#define _POSIX_C_SOURCE 1

#include "chgrp.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <grp.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int 
atou_s (const char *a, unsigned *p_u)
{
    unsigned u = 0;
    const char *iter = a;

    if ((a == NULL) || (p_u == NULL))
    { 
        return (errno = EINVAL);
    }

    /* ensure that the string is all digits */
    for (; isdigit (*iter); iter++) { }
    if (*iter != '\0') 
    { 
        return (errno = EINVAL);
    };

    /* convert to unsigned */
    for (; *a; a++)
    {
        u *= 10;
        u += '0' - *a;
    }

    *p_u = u;
    return 0;
}


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
    if (0 == atou_s (group_in, &gid))
    {
        *p_gid = gid;
        return 0;
    }

    return errno;
}

int
chgrp (gid_t gid, const char *path, int recurse)
{
    char child_path[PATH_MAX+1];

    int rc = 0;
    struct stat stbuf = { 0 };
    DIR *dfd = NULL;
    struct dirent *dp = NULL;

    /* change group */
    if (chown (path, -1, gid))
    {
        rc = 1;
        fprintf (stderr, "chgrp: cannot access: '%s': %s\n", path, strerror (errno));
    }

    /* and do nothing more if non recursive */
    if (!recurse) { return rc; }

    /* otherwise, on a recursive run, stat the file */
    if ((rc = stat (path, &stbuf)))
    {
        rc = 1;
        fprintf (stderr, "chgrp: cannot access: '%s': %s\n", path, strerror (errno));
        return rc;
    }

    /* if it is not a directory, dont search through it */
    if (!S_ISDIR (stbuf.st_mode)) { return rc; }

    /* but if it is! loop through it, recurively */
    dfd = opendir (path);
    if (dfd == NULL)
    {
        rc = 1;
        fprintf (stderr, "chgrp: cannot search: '%s': %s\n", path, strerror (errno));
        return rc;
    }

    while ((dp = readdir (dfd)) != NULL)
    {
        /* dont traverse special directories */
        if ((0 == strcmp (dp->d_name, ".")) ||
            (0 == strcmp (dp->d_name, "..")))
        {
            continue;
        }

        *child_path = '\0';
        (void)strncat (child_path, path, PATH_MAX);
        (void)strncat (child_path, "/", PATH_MAX);
        (void)strncat (child_path, dp->d_name, PATH_MAX);
        rc |= chgrp (gid, child_path, recurse);
    }

    closedir (dfd);
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

    /* get options */
    while ((c = getopt (argc, argv, "R")) != -1)
    {
        switch (c)
        {
        case 'R':
            recurse = 1;
            break;

        default:
            fprintf (stderr, "usage: chgrp [-R] group file [file...]\n");
            return 1;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 2)
    {
        fprintf (stderr, "usage: chgrp [-R] group file [file...]\n");
        return 1;
    }
    group_in = argv[0];
    files = &argv[1];
    file_count = argc-1;

    /* get group id */
    if (get_gid_s (group_in, &gid))
    {
        fprintf (stderr, "chgrp: invalid group: '%s'\n", group_in);
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
