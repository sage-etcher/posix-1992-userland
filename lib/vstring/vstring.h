
#ifndef VSTRING_H
#define VSTRING_H
#ifdef __cpluscplus
extern "C" {
#endif

#include <stdarg.h>
#include <stddef.h>

int vstrncat_auto (char *buf, size_t n, size_t i, va_list args);
size_t strncat_auto (char *buf, size_t n, size_t i, ...);
size_t strcat_auto (char *buf, size_t i, ...);


#ifdef __cpluscplus
}
#endif
#endif
/* end of file */
