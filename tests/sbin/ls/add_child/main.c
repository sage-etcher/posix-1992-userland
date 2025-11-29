
#include "ls.h"
#include "tap.h"

int
main (int argc, char **argv)
{
    const char *path = NULL;

    path = add_child (NULL, NULL);
    is_str(path, "/", "handles null parent and child") or diag("result path: %s", path);

    path = add_child ("/etc", NULL);
    is_str(path, "/etc/", "handles null child") or diag("result path: %s", path);

    path = add_child (NULL, "etc");
    is_str(path, "/etc", "handles null parent") or diag("result path: %s", path);

    path = add_child ("/etc", "test");
    is_str(path, "/etc/test", "intended function") or diag("result path: %s", path);

    done_testing();
    return exit_status();
}

/* end of file */
