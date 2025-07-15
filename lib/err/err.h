
#ifndef ERR_H
#define ERR_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <string.h>

extern const char *g_err_prefix;

void verr_prefix (const char *prefix, const char *fmt, va_list args);
void err_prefix (const char *prefix, const char *fmt, ...);

#define ERR_USAGE(usage_str)    err_prefix ("usage", usage_str)
#define ERR_ACCESS(path,err)    err_prefix (g_err_prefix, "cannot access: '%s': %s", path, strerror (err))
#define ERR_INVALID_GROUP(grp)  err_prefix (g_err_prefix, "invalid group: '%s'", grp)


#ifdef __cplusplus
}
#endif
#endif
/* end of file */
