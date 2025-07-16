
#ifndef SF_LS_H
#define SF_LS_H

#include <grp.h>
#include <pwd.h>
#include <sys/types.h>
#include <stddef.h>

char *add_child (const char *dir, const char *child);
int dir_content (char **buf, size_t n, const char dirname[]);
char *get_user_name (uid_t uid);
char *get_group_name (gid_t gid);

int ls_main (int argc, char **argv);

#endif
/* end of file */
