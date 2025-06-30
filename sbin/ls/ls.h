
#ifndef SF_LS_H
#define SF_LS_H

#include <stddef.h>

size_t filter (void *arr, size_t n, size_t elem_size, int (*cb)(void *a));
void map (void *arr, size_t n, size_t elem_size, void (*cb)(void *a));

size_t lu_len (long unsigned lu);
size_t snprintlu (char *buf, size_t n, long unsigned lu);
char *snprintlu_dup (long unsigned lu);
char *strdup (char *src);

int ls_main (int argc, char **argv);

#endif
/* end of file */
