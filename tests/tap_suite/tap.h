
#ifndef TAP_SUITE_H
#define TAP_SUITE_H
#if __cplusplus
extern "C" {
#endif

#include <iso646.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>


void plan(int number_of_tests);
void done_testing(void);

int ok(int condition, const char *test_name);

#define is(a,b,test_name)   ok(((a) == (b)), test_name)
#define isnt(a,b,test_name) ok(((a) != (b)), test_name)

int strcmp_null_safe (const char *a, const char *b);
#define is_str(a,b,test_name)   ok((strcmp_null_safe (a, b) == 0), test_name)
#define isnt_str(a,b,test_name) ok((strcmp_null_safe (a, b) != 0), test_name)

int memcmp_null_safe (const void *a, const void *b, size_t n);
#define is_mem(a,b,n,test_name)   ok((memcmp_null_safe (a, b, n) == 0), test_name)
#define isnt_mem(a,b,n,test_name) ok((memcmp_null_safe (a, b, n) != 0), test_name)

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
