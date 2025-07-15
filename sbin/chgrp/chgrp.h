
#ifndef CHGRP_H
#define CHGRP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

int get_gid_s (const char *group_in, gid_t *p_gid);
int chgrp (gid_t gid, const char *path, int recurse);
int chgrp_main (int argc, char **argv);


#ifdef __cplusplus
}
#endif
#endif
/* end of file */
