
#ifndef CONVERT_H
#define CONVERT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>


int atolu_s (const char *a, long unsigned *p_lu, short unsigned base);
int atou_s (const char *a, unsigned *p_u, short unsigned base);

size_t lulen (long unsigned lu, short unsigned base);
int lutoa_s (char *buf, size_t n, long unsigned lu, short unsigned base);

#ifdef __cplusplus
}
#endif
#endif
/* end of file */
