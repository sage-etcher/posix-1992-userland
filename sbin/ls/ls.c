
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

    PRINT_MODES = COLUMN_MODE | LONG_MODE | INODE_MODE | SINGLE_MODE,
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
    time_t time;
} file_info_t;

static int s_conf = 0;

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
            DISABLE_BFLAG (config, PRINT_MODES);
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

        case 'u':
            ENABLE_BFLAG (config, FILE_ACCESS);
            break;

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

char *
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

char *
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

    if (buf[0] == '?')
    {
        fprintf (stderr, "ls: unkown type of file\n");
        return NULL;
    }

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

    return sprintf_dup ("%s", s_buf);
    /* }}} */
}

char *
get_file_name (char *name, mode_t mode)
{
    char file_suffix = ' ';
    if (!(s_conf & SUFFIXES)) goto exit;

    if (S_ISFIFO (mode)) file_suffix = '|';
    else if (S_ISDIR (mode)) file_suffix = '/';
    else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) file_suffix = '*';

exit:
    return sprintf_dup ("%s%c", name, file_suffix);
}

int
file_info_new (file_info_t *self, const char *filepath, struct dirent *item)
{
    /* {{{ */
    struct stat header = { 0 };

    if (0 != stat (filepath, &header))
    {
        perror ("cannot stat file");
        return -1;
    }

    self->inode = sprintf_dup ("%lu", item->d_ino);
    self->mode  = get_file_mode (header.st_mode);
    self->nlink = sprintf_dup ("%lu", header.st_nlink);
    self->owner = get_user_name (header.st_uid);
    self->group = get_group_name (header.st_gid);
    self->size  = sprintf_dup ("%ld", header.st_size);
    self->name  = get_file_name (item->d_name, header.st_mode);

    self->time  = (s_conf & FILE_ACCESS) ? header.st_atime 
                                         : header.st_mtime;

    self->date = get_date (self->time);

    return 0;
    /* }}} */
}

void
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

size_t
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

char *
add_child (const char *dir, const char *child)
{
    /* {{{ */
    static char s_result[PATH_MAX+1] = { 0 };
    (void)snprintf (s_result, PATH_MAX+1, "%s/%s", dir, child);
    return s_result; 
    /* }}} */
}

int
dir_content (file_info_t *buf, size_t n, const char dirname[])
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

        file_info_new (buf++, add_child (dirname, dirent->d_name), dirent);
    }

    closedir (dir);
    return count;
    /* }}} */
}

int
long_mode (file_info_t *files, size_t file_count, const char *dir)
{
    /* {{{ */
    int max_widths[7] = {0};

    file_info_t *iter = NULL;
    size_t i = 0;


    for (i = 0, iter = files; i < file_count; i++, iter++)
    {
        max_widths[0] = MAX (max_widths[0], (int)strlen (iter->mode));
        max_widths[1] = MAX (max_widths[1], (int)strlen (iter->nlink));
        max_widths[2] = MAX (max_widths[2], (int)strlen (iter->owner));
        max_widths[3] = MAX (max_widths[3], (int)strlen (iter->group));
        max_widths[4] = MAX (max_widths[4], (int)strlen (iter->size));
        max_widths[5] = MAX (max_widths[5], (int)strlen (iter->date));
        max_widths[6] = MAX (max_widths[6], (int)strlen (iter->name));
    }

    for (i = 0, iter = files; i < file_count; i++, iter++)
    {
        printf ("%*s %*s %*s %*s %*s %*s %.*s\n",
                max_widths[0], iter->mode, 
                max_widths[1], iter->nlink, 
                max_widths[2], iter->owner, 
                max_widths[3], iter->group, 
                max_widths[4], iter->size, 
                max_widths[5], iter->date, 
                max_widths[6], iter->name);
    }

    return 0;
    /* }}} */
}

int 
sort_alphabetical (const void *a, const void *b)
{
    /* {{{ */
    const file_info_t *file0 = a;
    const file_info_t *file1 = b;
    int result = strcmp (file0->name, file1->name);
    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

int 
sort_date (const void *a, const void *b)
{
    /* {{{ */
    const file_info_t *file0 = a;
    const file_info_t *file1 = b;
    int result = -(int)difftime (file0->time, file1->time);
    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

int
ls_main (int argc, char **argv)
{
    /* {{{ */

    const char *columns     = getenv ("COLUMNS");
    const char *lang        = getenv ("LANG");
    const char *lc_all      = getenv ("LC_ALL");
    const char *lc_collate  = getenv ("LC_COLLATE");
    const char *lc_ctype    = getenv ("LC_CTYPE");
    const char *lc_messages = getenv ("LC_MESSAGES");
    const char *lc_time     = getenv ("LC_TIME");
    const char *tz          = getenv ("TZ");

    char *dir = NULL;
    file_info_t *files = NULL;
    size_t file_count = 0;

    int n = get_config (argc, argv, &s_conf);

    if (n <= 0) return -1;

    /* set argc, argv to the first non-option argument */
    argc -= n;
    argv += n;

    dir = *argv == NULL ? "." : *argv;

    file_count = dir_content (NULL, 0, dir);
    files = malloc (file_count * sizeof (*files));
    (void)dir_content (files, file_count, dir);

    if (s_conf & SORT_TIME)
    {
        qsort (files, file_count, sizeof (*files), sort_date);
    }
    else
    {
        qsort (files, file_count, sizeof (*files), sort_alphabetical);
    }

    long_mode (files, file_count, dir);

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
