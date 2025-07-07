
#ifndef TAP_SUITE_H
#define TAP_SUITE_H
#if __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>

void plan(int number_of_tests);

int ok(int condition, const char *test_name);

#define is(a,b,test_name)   ok(((a) == (b)), test_name)
#define isnt(a,b,test_name) ok(((a) != (b)), test_name)

#define is_str(a,b,test_name)   ok((strcmp (a, b) == 0), test_name)
#define isnt_str(a,b,test_name) ok((strcmp (a, b) != 0), test_name)

#define is_mem(a,b,n,test_name)   ok((memcmp (a, b, n) == 0), test_name)
#define isnt_mem(a,b,n,test_name) ok((memcmp (a, b, n) != 0), test_name)

int pass(const char *test_name);
int fail(const char *test_name);
void skip(int number, const char *reason);

int diag(const char *diagnostic_message, ...);
int note(const char *diagnostic_message, ...);

int exit_status(void);
void bail_out(const char *reason);

#if __cplusplus
}
#endif
#endif
/* end of file */
