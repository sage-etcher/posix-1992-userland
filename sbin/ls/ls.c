
#define _POSIX_C_SOURCE 2

#include <assert.h>
#include <linux/limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* settng flags */
enum { 
    /* {{{ */
    COLUMN_MODE = 1 << 1,
    SUFFIXES    = 1 << 2,
    RECURSIVE   = 1 << 3,
    HIDDEN      = 1 << 4,
    FILE_STATUS = 1 << 5,
    NO_DIRECTORY= 1 << 6,
    INODE_MODE  = 1 << 7,
    LONG_MODE   = 1 << 8,
    PRINTABLE   = 1 << 9,
    SORT_REVERSE= 1 << 10,
    SORT_TIME   = 1 << 11,
    FILE_ACCESS = 1 << 12,
    SINGLE_MODE = 1 << 13,

    PRINT_MODES = COLUMN_MODE | LONG_MODE | SINGLE_MODE,
    TIME_MODES  = FILE_STATUS | FILE_ACCESS,
    /* }}} */
}; 

#define ENABLE_BFLAG(var, flag)     ((var) |= (flag))
#define DISABLE_BFLAG(var, flag)    ((var) &= ~(flag))
#define IS_BFLAG(var, flag)         ((var) & (flag))

#define MAX(x,y) ((x) > (y) ? (x) : (y))

typedef struct {
    char *inode;
    char *mode;
    char *nlink;
    char *owner;
    char *group;
    char *size;
    char *date;
    char *name;
    char suffix;
    time_t time;
    size_t blocks;
} file_info_t;

typedef struct {
    const char *filename;
    struct stat stat;
    time_t time;
} file_stats_t;


static int s_conf = 0;

int ls_main (int argc, char **argv);

size_t filter (void *arr, size_t elem_count, size_t elem_size, int (*cb)(void *a));
void map (void *arr, size_t elem_count, size_t elem_size, void (*cb)(void *a));
char *sprintf_dup (const char *fmt, ...);
char *strdup (const char *src);

static char *add_child (const char *dir, const char *child);
static double check_future (time_t file_time, time_t now);
static double check_oldest (time_t file_time, time_t now);
static int column_mode (file_stats_t *files, size_t file_count, const char *dir);
static int dir_content (char **buf, size_t n, const char dirname[]);
static size_t dir_size (const char dirname[]);
static void file_info_destroy (file_info_t *self);
static int get_config (int argc, char **argv, int *p_conf);
static char *get_date (time_t file_time);
static char *get_file_mode (mode_t file_mode);
static char *get_group_name (gid_t gid);
static char *get_user_name (uid_t uid);
static int list_directory (const char *dir);
static int list_directories (const char **dirs, size_t n, int first);
static int long_mode (file_stats_t *files, size_t file_count, const char *dir);
static int print_file_list (file_info_t *files, size_t file_count, const char *dir);
static int single_mode (file_stats_t *files, size_t file_count, const char *dir);

static int file_info_new (file_info_t *self, const char *filepath, const char *filename);
static int filter_hidden (void *a);
static void map_free_file_info (void *a);
static void map_free_str_array (void *a);
static int sort_alphabetical (const void *a, const void *b);
static int sort_date (const void *a, const void *b);


/* generic */
size_t
filter (void *arr, size_t elem_count, size_t elem_size, int (*cb)(void *a))
{
    /* {{{ */
    unsigned char *src = arr;
    unsigned char *dst = arr;
    size_t count = 0;

    for (; elem_count > 0; elem_count--)
    {
        if (cb (src))
        {
            (void)memcpy (dst, src, elem_size);
            dst += elem_size;
            count++;
        }
        src += elem_size;
    }

    return count;
    /* }}} */
}

void
map (void *arr, size_t elem_count, size_t elem_size, void (*cb)(void *a))
{
    /* {{{ */
    unsigned char *iter = arr;

    for (; elem_count > 0; elem_count--)
    {
        cb (iter);
        iter += elem_size;
    }
    /* }}} */
}

char *
sprintf_dup (const char *fmt, ...)
{
    /* {{{ */
    char *buf = NULL;
    size_t n = 0;

    va_list args, args_copy;
    va_start (args, fmt);
    va_copy (args_copy, args);

    n = vsnprintf (NULL, 0, fmt, args);
    buf = malloc (n + 1);
    assert (buf != NULL);

    (void)vsnprintf (buf, n+1, fmt, args_copy);

    va_end (args);
    va_end (args_copy);

    return buf;
    /* }}} */
}

#ifndef strdup
char *
strdup (const char *src)
{
    /* {{{ */
    return sprintf_dup ("%s", src);
    /* }}} */
}
#endif


/* unix stuff */
static char *
add_child (const char *dir, const char *child)
{
    /* {{{ */
    static char s_result[PATH_MAX+1] = { 0 };
    (void)snprintf (s_result, PATH_MAX+1, "%s/%s", dir, child);
    return s_result; 
    /* }}} */
}


/* ls callbacks */
static int 
sort_alphabetical (const void *a, const void *b)
{
    /* {{{ */
    const file_stats_t *file0 = a;
    const file_stats_t *file1 = b;
    int result = strcmp (file0->filename, file1->filename);
    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

static int 
sort_date (const void *a, const void *b)
{
    /* {{{ */
    const file_stats_t *file0 = a;
    const file_stats_t *file1 = b;
    int result = -(int)difftime (file0->time, file1->time);
    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

static int
filter_hidden (void *a)
{
    /* {{{ */
    char **file = a;
    return (**file != '.');
    /* }}} */
}

static void
map_select_date (void *a)
{
    file_stats_t *self = a;
    if (s_conf & FILE_ACCESS)
    {
        self->time = self->stat.st_atime;
    }
    else
    {
        self->time = self->stat.st_mtime;
    }
}

static void
map_free_file_info (void *a)
{
    /* {{{ */
    file_info_destroy (a);
    /* }}} */
}

static void
map_free_str_array (void *a)
{
    char **p_str = a;
    free (*p_str);
}


/* ls exclusive */
static char *
get_user_name (uid_t uid)
{
    /* {{{ */
    struct passwd *user = NULL;
    const char *name = NULL;

    /* get data about the user */
    user = getpwuid (uid);

    /* if the the user cannot be found */
    if (NULL == user)
    {
        return sprintf_dup ("%u", uid);
    }

    return sprintf_dup ("%s", user->pw_name);
    /* }}} */
}

static char *
get_group_name (gid_t gid)
{
    /* {{{ */
    struct group *group = NULL;
    const char *name = NULL;

    /* get data about the group */
    group = getgrgid (gid);

    /* if the the group cannot be found */
    if (NULL == group)
    {
        return sprintf_dup ("%u", gid);
    }

    return sprintf_dup ("%s", group->gr_name);
    /* }}} */
}

static char *
get_file_mode (mode_t file_mode)
{
    /* {{{ */
    char buf[12] = { 0 };
    
    /* type of file */
    buf[0] = (S_ISDIR (file_mode)  ? 'd' :
              S_ISBLK (file_mode)  ? 'b' :
              S_ISCHR (file_mode)  ? 'c' :
              S_ISFIFO (file_mode) ? 'p' : 
              S_ISREG (file_mode)  ? '-' : '?');

    /* owner file perms */
    buf[1] = (file_mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (file_mode & S_IWUSR) ? 'w' : '-';

    if (file_mode & S_ISUID)
    { 
        buf[3] = (file_mode & S_IXUSR) ? 'S' : 's';
    }
    else
    {
        buf[3] = (file_mode & S_IXUSR) ? 'x' : '-';
    }

    /* group file perms */
    buf[4] = (file_mode & S_IRGRP ? 'r' : '-');
    buf[5] = (file_mode & S_IWGRP ? 'w' : '-');
    buf[6] = (file_mode & S_IXGRP ? 'x' : '-');

    /* other file perms */
    buf[7] = (file_mode & S_IROTH ? 'r' : '-');
    buf[8] = (file_mode & S_IWOTH ? 'w' : '-');
    buf[9] = (file_mode & S_IXOTH ? 'x' : '-');

    /* alternate access method flag */
    buf[10] = ' ';
    buf[11] = '\0';

    return sprintf_dup ("%s", buf);
    /* }}} */
}

static double 
check_oldest (time_t file_time, time_t now)
{
    /* {{{ */
    struct tm *oldest_date = NULL;
    time_t oldest_time = 0;

    oldest_date = localtime (&now);
    if (oldest_date->tm_mon < 6)
    {
        oldest_date->tm_year--;
        oldest_date->tm_mon += 12;
    }
    oldest_date->tm_mon -= 6;
    oldest_time = mktime (oldest_date);

    return difftime (file_time, oldest_time);
    /* }}} */
}

static double 
check_future (time_t file_time, time_t now)
{
    /* {{{ */
    return difftime (now, file_time);
    /* }}} */
}

static char *
get_date (time_t file_time)
{
    /* {{{ */
    static char s_buf[100];
    const char *date_format = NULL;
    const char *posix_new_fmt = "%b %e %H:%M";
    const char *posix_old_fmt = "%b %e  %Y";
    time_t now = time (NULL);

    if ((check_future (file_time, now) < 0) || 
        (check_oldest (file_time, now) <= 0))
    {
        date_format = posix_old_fmt;
    }
    else
    {
        date_format = posix_new_fmt;
    }

    strftime (s_buf, 100, date_format, localtime (&file_time));

    return sprintf_dup ("%s", s_buf);
    /* }}} */
}

static char
get_file_suffix (mode_t mode)
{
    /* {{{ */
    if (!(s_conf & SUFFIXES)) 
    {
        return ' ';
    }
    if (S_ISFIFO (mode)) 
    {
        return '|';
    }
    if (S_ISDIR (mode)) 
    {
        return '/';
    }
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) 
    {
        return '*';
    }
    
    return ' ';
    /* }}} */
}

static int
file_info_new (file_info_t *self, const char *filepath, const char *filename)
{
    /* {{{ */
    struct stat header = { 0 };

    if (0 != stat (filepath, &header))
    {
        perror ("cannot stat file");
        return -1;
    }

    self->inode = sprintf_dup ("%lu", header.st_ino);
    self->mode  = get_file_mode (header.st_mode);
    self->nlink = sprintf_dup ("%lu", header.st_nlink);
    self->owner = get_user_name (header.st_uid);
    self->group = get_group_name (header.st_gid);
    self->size  = sprintf_dup ("%ld", header.st_size);
    self->name  = sprintf_dup ("%s", filename);
    self->suffix = get_file_suffix (header.st_mode);

    self->time  = (s_conf & FILE_ACCESS) ? header.st_atime 
                                         : header.st_mtime;

    self->date = get_date (self->time);

    self->blocks = header.st_blocks;

    return 0;
    /* }}} */
}

static void
file_info_destroy (file_info_t *self)
{
    /* {{{ */
    if (self == NULL) return;

    free (self->inode);
    free (self->mode);
    free (self->nlink);
    free (self->owner);
    free (self->group);
    free (self->size);
    free (self->date);
    free (self->name);
    /* }}} */
}

static size_t
dir_size (const char dirname[])
{
    /* {{{ */
    DIR *dir = opendir (dirname);
    struct dirent *dirent = NULL;
    size_t count = 0;

    /* iterate over the directory contents */
    while ((dirent = readdir (dir)) != NULL)
    {
        count++;
    }

    closedir (dir);
    return count;
    /* }}} */
}

static int
dir_content (char **buf, size_t n, const char dirname[])
    
{
    /* {{{ */
    DIR *dir = opendir (dirname);
    struct dirent *dirent = NULL;
    size_t count = 0;
  
    if (dir == NULL)
    {
        perror ("cannot read directory");
        return -1;
    }

    /* iterate over the directory contents */
    while ((dirent = readdir (dir)) != NULL)
    {
        count++;
        if (buf == NULL) continue;
        if (count > n) 
        { 
            closedir (dir);
            return count; 
        }

        *buf = strdup (dirent->d_name);
        //file_info_new (buf++, add_child (dirname, dirent->d_name), dirent->d_name);
    }

    closedir (dir);
    return count;
    /* }}} */
}

typedef struct {
    char mode[11]; /* "drwxrwxrwx " */
    unsigned long nlink;
    char *owner;
    char *group;
    unsigned long size;
    char date[12]; /* "MON DD  YYYY" or "MON DD HH:MM" */
    char *file;
} long_mode_fmt_t;

static int
long_mode (file_stats_t *files, size_t n, const char *dir)
{
    /* {{{ */

    size_t i = 0;
    long_mode_fmt_t *fmt = malloc (n * sizeof (long_mode_fmt_t));

    for (i = 0; i > n; i++)
    {
        get_long_mode_fmt (&fmt[i], files[i]);
    }




    return 0;
    /* }}} */
}

static int 
column_mode (file_stats_t *files, size_t n, const char *dir)
{
    /* {{{ */
    printf ("[todo: column_mode()]\n");
    return -1;
    /* }}} */
}

static int 
single_mode (file_stats_t *files, size_t file_count, const char *dir)
{
    /* {{{ */
    int max_widths[1] = {0};

    file_info_t *iter = NULL;
    size_t i = 0;

    for (i = 0, iter = files; i < file_count; i++, iter++)
    {
        max_widths[0] = MAX (max_widths[0], (int)strlen (iter->inode));
    }

    for (i = 0, iter = files; i < file_count; i++, iter++)
    {
        if (s_conf & INODE_MODE)
        {
            printf ("%*s ", max_widths[0], iter->inode);
        }

        printf ("%s%c\n",
                iter->name,
                iter->suffix);
    }

    return 0;
    /* }}} */
}

static int
get_config (int argc, char **argv, int *p_conf)
{ 
    /* {{{ */
    int config = SINGLE_MODE | FILE_STATUS;
    int c = 0;

    assert (p_conf != NULL);

    while (-1 != (c = getopt (argc, argv, "CFRacdilqrtu1")))
    {
        switch (c)
        {
        case 'C':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, COLUMN_MODE);
            break;

        case 'F':
            ENABLE_BFLAG (config, SUFFIXES);
            break;

        case 'R':
            ENABLE_BFLAG (config, RECURSIVE);
            break;

        case 'a':
            ENABLE_BFLAG (config, HIDDEN);
            break;

        case 'c':
            DISABLE_BFLAG (config, TIME_MODES);
            ENABLE_BFLAG (config, FILE_STATUS);
            break;

        case 'd':
            ENABLE_BFLAG (config, NO_DIRECTORY);
            break;

        case 'i':
            ENABLE_BFLAG (config, INODE_MODE);
            break;

        case 'l':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, LONG_MODE);
            break;

        case 'r':
            ENABLE_BFLAG (config, SORT_REVERSE);
            break;

        case 't':
            ENABLE_BFLAG (config, SORT_TIME);
            break;

        case 'u': ENABLE_BFLAG (config, FILE_ACCESS); break;
        case '1':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, SINGLE_MODE);
            break;

        case '?':
        default:
            (void)printf ("usage: ls [-CFRacdilqrtu1][file...]\n");
            return -1;
        }

    }

exit:
    *p_conf = config;
    return optind;
    /* }}} */
}


static file_stats_t *
get_file_stats (char **files, size_t n, const char *dir)
{
    file_stats_t *self = malloc (n * sizeof (file_stats_t));

    for (; n > 0; n--)
    {
        self[n-1].filename = files[n-1];
        stat (add_child (dir, files[n-1]), &self[n-1].stat);
    }

    return self;

}


static int
print_files (file_stats_t *contents, size_t n, const char *dir)
{
    /* {{{ */
    /* sort the outputs */
    qsort (contents, n, sizeof (*contents), sort_alphabetical);
    if (s_conf & SORT_TIME)
    {
        qsort (contents, n, sizeof (*contents), sort_date);
    }

    /* print the results */
    if (s_conf & LONG_MODE)
    {
        long_mode (contents, n, dir);
    }
    else if (s_conf & COLUMN_MODE)
    {
        column_mode (contents, n, dir);
    }
    else if (s_conf & SINGLE_MODE)
    {
        single_mode (contents, n, dir);
    }
    else 
    {
        single_mode (contents, n, dir);
    }

    return 0;
    /* }}} */
}

static int
list_directory (const char *dir)
{
    /* {{{ */
    int rc = 0;
    size_t i = 0;

    /* get contents */
    size_t n = dir_size (dir);
    char **files = malloc (n * sizeof (char *));
    file_stats_t *contents = NULL;

    (void)dir_content (files, n, dir);

    /* remove hidden files */
    if (!(s_conf & HIDDEN))
    {
        n = filter (files, n, sizeof (*files), filter_hidden);
    }

    /* get the stats */
    contents = get_file_stats (files, n, dir);
    map (contents, n, sizeof (*contents), map_select_date);

    /* log the results */
    print_files (contents, n, dir);

    /* clean up */
    map (files, n, sizeof (*files), map_free_str_array);
    free (contents);
    free (files);
    return rc;
    /* }}} */
}

static int 
list_directories (const char **dirs, size_t n, int first)
{
    /* {{{ */
    for (; n > 0; n--, dirs++)
    {
        if (n > 1)
        {
            if (!first) printf ("\n");
            printf ("%s:\n", *dirs);
        }

        (void)list_directory (*dirs);

        first = 0;
    }

    return 0;
    /* }}} */
}

/* PUBlIC */
int
ls_main (int argc, char **argv)
{
    /* {{{ */
    const char *columns = getenv ("COLUMNS");

    char *dir = NULL;
    int dir_count = 0;

    int n = get_config (argc, argv, &s_conf);
    if (n <= 0) return -1;
    argc -= n; argv += n;
    
    if (argc <= 0)
    {
        argv = (char **)&".";
        argc = 1;
    }

    list_directories ((const char **)argv, argc, 1);

    return 0;

    dir = *argv;
    dir_count = argc;
   


    do 
    {
        if (s_conf & NO_DIRECTORY)
        {
            file_count = dir_count;
            files = malloc (file_count * sizeof (*files));

            i = 0;
            do
            {
                file_info_new (&files[i], dir, dir);
                i++;
                dir = argv[i];
            }
            while (i < argc);

            argc = 0;
        }
        else 
        {
            list_directory (dir);
        }

        argc--;
        argv++;
        dir = *argv;

        if ((argc > 0) && (!(s_conf & NO_DIRECTORY)))
        {
            printf ("\n");
        }

    } 
    while (argc > 0);

    /* free files */

    return 0;
    /* }}} */
}

int
main (int argc, char **argv)
{
    /* {{{ */
    return ls_main (argc, argv);
    /* }}} */
}

/* vim: fdm=marker
 * end of file */
