
#ifndef CHMOD_H
#define CHMOD_H
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

int chmod_handler (mode_t mode, const char *path, int recurse);
int chmod_main (int argc, char **argv);


#ifdef __cplusplus
}
#endif
#endif
/* end of file */
