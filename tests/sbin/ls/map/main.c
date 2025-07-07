
#include "ls.h"
#include "tap.h"

int
main (int argc, char **argv)
{
    char buf[10] = { 0 };

    plan(4);
    is('a'+1, 'b', "sanity check 'a'+1 == 'b'");
    is('y'+1, 'z', "sanity check 'y'+1 == 'z'");
    
    strncpy (buf, "abcdefg", 10);
    map (buf, strlen (buf), sizeof (*buf), map_to_upper, NULL);

    return exit_code();
}

/* end of file */
