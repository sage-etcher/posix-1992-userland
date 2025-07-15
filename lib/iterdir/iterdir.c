
#define _POSIX_C_SOURCE 1

#include "iterdir.h"

#include "err.h"

#include <dirent.h>
#include <errno.h>
#include <grp.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>


static DIR *s_dfd = NULL;

void
iterdir_done (void)
{
    closedir (s_dfd);
    s_dfd = NULL;
}

struct dirent *
iterdir (const char *path)
{
    if (path != NULL)
    {
        iterdir_done ();
        s_dfd = opendir (path);
        if (s_dfd == NULL) { return NULL; }
    }

    if (s_dfd == NULL)
    {
        errno = EINVAL;
        return NULL;
    }

    return readdir (s_dfd);
}

int
is_special_dir (const char *name)
{
    return ((0 == strcmp (name, ".")) || 
            (0 == strcmp (name, "..")));
}

int
is_dir (const char *path)
{
    int rc = 0;
    struct stat stbuf = { 0 };

    if (stat (path, &stbuf))
    {
        rc = errno;
        ERR_ACCESS (path, errno);
    }
    
    errno = rc;
    return S_ISDIR (stbuf.st_mode);
}

/* end of file */
