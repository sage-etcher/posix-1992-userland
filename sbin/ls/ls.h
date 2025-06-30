
#ifndef SF_LS_H
#define SF_LS_H

#include <grp.h>
#include <pwd.h>
#include <stddef.h>

size_t filter (void *arr, size_t elem_count, size_t elem_size, int (*cb)(void *a));
void map (void *arr, size_t elem_count, size_t elem_size, void (*cb)(void *a));
char *strdup (const char *src);
size_t lu_len (long unsigned lu);
size_t snprintlu (char *buf, size_t n, long unsigned lu);
char *snprintlu_dup (long unsigned lu);

char *add_child (const char *dir, const char *child);
int dir_content (char **buf, size_t n, const char dirname[]);
char *get_user_name (uid_t uid);
char *get_group_name (gid_t gid);

int ls_main (int argc, char **argv);

#endif
/* end of file */


