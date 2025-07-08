
#include "ls.h"
#include "tap.h"

int
main (int argc, char **argv)
{
    const char *path = NULL;

    path = add_child (NULL, NULL);
    is_str(path, "/", "NULL parent and child") or
    diag("result path: %s", path);

    path = add_child ("/etc", NULL);
    is_str(path, "/etc/", "NULL child, valid parent") or
    diag("result path: %s", path);

    path = add_child (NULL, "etc");
    is_str(path, "/etc", "NULL parent, valid child") or
    diag("result path: %s", path);

    path = add_child ("/etc", "test");
    is_str(path, "/etc/test", "valid parent and child") or
    diag("result path: %s", path);

    done_testing();
    return exit_status();
}

/* end of file */
