
#ifndef ITERDIR_H
#define ITERDIR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>


struct dirent *iterdir (const char *path);
void iterdir_done (void);

int is_dir (const char *path);
int is_special_dir (const char *name);


#ifdef __cplusplus
}
#endif
#endif

/* end of file */
