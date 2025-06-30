
#define _POSIX_C_SOURCE 2

#include <assert.h>
#include <linux/limits.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
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
    TIME_MODES  = FILE_STATUS | FILE_ACCESS
    /* }}} */
}; 

#define ENABLE_BFLAG(var, flag)     ((var) |= (flag))
#define DISABLE_BFLAG(var, flag)    ((var) &= ~(flag))
#define IS_BFLAG(var, flag)         ((var) & (flag))

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define UNUSED(x) ((void)(x))

typedef struct {
    char *filename;
    struct stat stat;
    time_t time;
} file_stat_t;
    
typedef struct {
    char *mode; /* char[12] = "drwxrwxrwx " */
    char *owner;
    char *group;
    char *date; /* char[13] = "Mon DD HH:MM" or "Mon DD  YYYY" */
} long_fmt_t;


static int s_conf = 0;

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
strdup (const char *src)
{
    /* {{{ */
    size_t n = strlen (src);
    char *dst = malloc (n + 1);
    return memcpy (dst, src, n+1);
    /* }}} */
}

size_t 
lu_len (long unsigned lu)
{
    /* {{{ */
    size_t n = 0;

    if (lu == 0) return 1;

    for (n = 0; lu > 0; lu /= 10, n++);

    return n;
    /* }}} */
}

size_t 
snprintlu (char *buf, size_t n, long unsigned lu)
{
    /* {{{ */
    size_t i = 0;
    size_t size = lu_len (lu);
    char *iter = &buf[n-1];

    for (i = 0; i < n; i++)
    {
        *iter = '0' + (lu % 10);
        iter--;
        lu /= 10;
    }

    buf[n] = '\0';

    return size;
    /* }}} */
}

char *
snprintlu_dup (long unsigned lu)
{
    /* {{{ */
    size_t size = lu_len (lu);
    char *buf = malloc (size + 1);
    (void)snprintlu (buf, size, lu);
    return buf;
    /* }}} */
}


/* unix stuff */
char *
add_child (const char *dir, const char *child)
{
    /* {{{ */
    static char s_result[PATH_MAX+1] = { 0 };
    char *iter = s_result;
    size_t len = 0;
    
    len = strlen (dir);
    memcpy (iter, dir, len);
    iter += len;

    *iter = '/';
    iter++;

    len = strlen (child);
    memcpy (iter, child, len);
    iter += len;

    *iter = '\0';

    return s_result; 
    /* }}} */
}

int
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
        
        *buf++ = strdup (dirent->d_name);
    }

    closedir (dir);
    return count;
    /* }}} */
}

char *
get_user_name (uid_t uid)
{
    /* {{{ */
    struct passwd *user = NULL;

    /* get data about the user */
    user = getpwuid (uid);

    /* if the the user cannot be found */
    if (NULL == user)
    {
        return snprintlu_dup (uid);
    }

    return strdup (user->pw_name);
    /* }}} */
}

char *
get_group_name (gid_t gid)
{
    /* {{{ */
    struct group *group = NULL;

    /* get data about the group */
    group = getgrgid (gid);

    /* if the the group cannot be found */
    if (NULL == group)
    {
        return snprintlu_dup (gid);
    }

    return strdup (group->gr_name);
    /* }}} */
}


/* ls callbacks */
int 
sort_alphabetical (const void *a, const void *b)
{
    /* {{{ */
    const file_stat_t *file0 = a;
    const file_stat_t *file1 = b;
    int result = strcmp (file0->filename, file1->filename);
    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

int 
sort_date (const void *a, const void *b)
{
    /* {{{ */
    const file_stat_t *file0 = a;
    const file_stat_t *file1 = b;
    int result = -(int)difftime (file0->time, file1->time);
    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

int
filter_hidden (void *a)
{
    /* {{{ */
    char **filename = a;
    int result = (**filename != '.');
    
    /* free non matches */
    if (!result)
    {
        free (*filename);
    }

    return result;
    /* }}} */
}

void
map_free_str_array (void *a)
{
    /* {{{ */
    free (*(char **)a);
    /* }}} */
}

void 
map_free_long_fmt (void *a)
{
    /* {{{ */
    long_fmt_t *fmt = a;
    free (fmt->date);
    free (fmt->group);
    free (fmt->mode);
    free (fmt->owner);
    /* }}} */
}


/* ls exclusive */
char *
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

    return strdup (buf);
    /* }}} */
}

double 
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

double 
check_future (time_t file_time, time_t now)
{
    /* {{{ */
    return difftime (now, file_time);
    /* }}} */
}

char *
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

    return strdup (s_buf);
    /* }}} */
}

char
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

int
long_mode (file_stat_t *stats, size_t n, const char *dir)
{
    /* {{{ */
    long_fmt_t *fmt = malloc (n * sizeof (long_fmt_t));
    int max_widths[7] = {0};
    size_t total_blocks = 0;

    file_stat_t *iter = NULL;
    size_t i = 0;

    UNUSED (dir);

    for (i = 0, iter = stats; i < n; i++, iter++)
    {
        fmt[i].mode  = get_file_mode (iter->stat.st_mode);
        fmt[i].owner = get_user_name (iter->stat.st_uid);
        fmt[i].group = get_group_name (iter->stat.st_gid);
        fmt[i].date  = get_date (iter->time);

        max_widths[0] = MAX (max_widths[0], (int)lu_len (iter->stat.st_ino));
        max_widths[1] = MAX (max_widths[1], (int)lu_len (iter->stat.st_nlink));
        max_widths[2] = MAX (max_widths[2], (int)strlen (fmt[i].owner));
        max_widths[3] = MAX (max_widths[3], (int)strlen (fmt[i].group));
        max_widths[4] = MAX (max_widths[4], (int)lu_len (iter->stat.st_size));

        total_blocks += iter->stat.st_blocks;
    }

    if (!(s_conf & NO_DIRECTORY))
    {
        printf ("total %lu\n", total_blocks);
    }

    for (i = 0, iter = stats; i < n; i++, iter++)
    {
        if (s_conf & INODE_MODE)
        {
            printf ("%*lu ", max_widths[0], iter->stat.st_ino);
        }

        printf ("%s %*lu %*s %*s %*lu %s %s%c\n",
                fmt[i].mode, 
                max_widths[1], iter->stat.st_nlink, 
                max_widths[2], fmt[i].owner, 
                max_widths[3], fmt[i].group, 
                max_widths[4], iter->stat.st_size, 
                fmt[i].date, 
                iter->filename,
                get_file_suffix (iter->stat.st_mode));
    }

    map (fmt, n, sizeof (*fmt), map_free_long_fmt);
    free (fmt);
    return 0;
    /* }}} */
}

int 
column_mode (file_stat_t *stats, size_t n, const char *dir)
{
    /* {{{ */
    UNUSED (stats);
    UNUSED (n);
    UNUSED (dir);
    printf ("[todo: column_mode()]\n");
    return -1;
    /* }}} */
}

int 
single_mode (file_stat_t *stats, size_t n, const char *dir)
{
    /* {{{ */
    int max_widths[1] = {0};

    file_stat_t *iter = NULL;
    size_t i = 0;

    UNUSED (dir);

    for (i = 0, iter = stats; i < n; i++, iter++)
    {
        max_widths[0] = MAX (max_widths[0], (int)lu_len (iter->stat.st_ino));
    }

    for (i = 0, iter = stats; i < n; i++, iter++)
    {
        if (s_conf & INODE_MODE)
        {
            printf ("%*lu ", max_widths[0], iter->stat.st_ino);
        }

        printf ("%s%c\n",
                iter->filename,
                get_file_suffix (iter->stat.st_mode));
    }

    return 0;
    /* }}} */
}

int
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

    *p_conf = config;
    return optind;
    /* }}} */
}

int
list_files (char **files, size_t n, char *dir)
{
    /* {{{ */
    size_t i = 0;
    file_stat_t *stats = malloc (n * sizeof (file_stat_t));

    /* get stats on all the files */
    for (i = 0; i < n; i++)
    {
        stats[i].filename = files[i];
        (void)stat (add_child (dir, files[i]), &stats[i].stat);

        /* time to use */
        if (s_conf & FILE_ACCESS)
        {
            stats[i].time = stats[i].stat.st_atime;
        }
        else if (s_conf & FILE_STATUS)
        {
            stats[i].time = stats[i].stat.st_ctime;
        }
        else 
        {
            stats[i].time = stats[i].stat.st_mtime;
        }
    }

    /* sort */
    if (s_conf & SORT_TIME)
    {
        qsort (stats, n, sizeof (*stats), sort_date);
    }
    else 
    {
        qsort (stats, n, sizeof (*stats), sort_alphabetical);
    }

    /* output */
    if (s_conf & LONG_MODE)
    {
        long_mode (stats, n, dir);
    }
    else if (s_conf & COLUMN_MODE)
    {
        column_mode (stats, n, dir);
    }
    else 
    {
        single_mode (stats, n, dir);
    }

    free (stats);
    return 0;
    /* }}} */
}

int
list_directory (char *dir)
{
    /* {{{ */
    size_t n_entries = dir_content (NULL, 0, dir);
    char **entries = malloc (n_entries * sizeof (char *));
    dir_content (entries, n_entries, dir);

    /* remove hidden files */
    if (!(s_conf & HIDDEN))
    {
        n_entries = filter (entries, n_entries, sizeof (*entries), filter_hidden);
    }

    /* list the contents */
    list_files (entries, n_entries, dir);

    map (entries, n_entries, sizeof (*entries), map_free_str_array);
    free (entries);

    return 0;
    /* }}} */
}

int
list_directories (char **dirs, size_t n, int first)
{
    /* {{{ */
    size_t i = 0;

    for (i = 0; i < n; i++, dirs++)
    {
        if (n > 0) 
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

int
ls_main (int argc, char **argv)
{
    /* {{{ */
    char *default_dir = ".";
    int n = get_config (argc, argv, &s_conf);

    if (n <= 0) return -1;
    argc -= n;
    argv += n;
    
    /* if no paths are given, default to current */
    if (argc <= 0)
    {
        argv = &default_dir;
        argc = 1;
    }

    /* list */
    if (s_conf & NO_DIRECTORY)
    {
        list_files (argv, argc, "");
    }
    else
    {
        list_directories (argv, argc, 1);
    }

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
