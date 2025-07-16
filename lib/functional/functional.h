
#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef int (*filter_cb_t)(void *data, void *iter);
typedef void (*map_cb_t)(void *data, void *iter);

int filter_s (void *tarr, size_t len, size_t tsize, filter_cb_t cb, void *data,
              size_t *p_new_len);
int map_s (void *tarr, size_t len, size_t tsize, map_cb_t cb, void *data);


#ifdef __cplusplus
}
#endif
#endif
/* end of file */
