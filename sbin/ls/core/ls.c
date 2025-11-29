
#define _POSIX_C_SOURCE 2

#include <assert.h>
#include <ctype.h>
#include <libintl.h>
#include <limits.h>
#include <locale.h>
#include <dirent.h>
#include <grp.h>
#include <math.h>
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
    FILE_MOD    = 1 << 14,
    SORT_COLLATE= 1 << 15,

    PRINT_MODES = COLUMN_MODE | LONG_MODE | SINGLE_MODE,
    TIME_MODES  = FILE_STATUS | FILE_ACCESS | FILE_MOD,
    SORT_MODES  = SORT_COLLATE | SORT_TIME
    /* }}} */
}; 

#define ENABLE_BFLAG(var, flag)     ((var) |= (flag))
#define DISABLE_BFLAG(var, flag)    ((var) &= ~(flag))
#define IS_BFLAG(var, flag)         ((var) & (flag))

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define UNUSED(x) ((void)(x))

#if !defined(DOMAIN_NAME) || !defined(DOMAIN_DIR)
#   warning using default values for DOMAIN_NAME and DOMAIN_DIR
#   define DOMAIN_NAME "ls"
#   define DOMAIN_DIR  "/usr/local/share/locale"
#endif

#define _(msg)    gettext (msg)

typedef struct {
    char *filename;
    char *printable;
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

static int sort_alphabetical (const void *a, const void *b);
static int sort_date (const void *a, const void *b);
static int filter_hidden (void *cb_data, void *a);
static int filter_non_directories (void *cb_data, void *a);
static int filter_special_directories (void *cb_data, void *a);
static void map_free_str_array (void *cb_data, void *a);
static void map_free_long_fmt (void *cb_data, void *a);
static void map_add_path (void *cb_data, void *a);
static void map_print_str_array (void *cb_data, void *a);

static char *get_printable_filename (char *src, mode_t mode);
static char *get_file_mode (mode_t file_mode);
static double check_oldest (time_t file_time, time_t now);
static double check_future (time_t file_time, time_t now);
static char *get_date (time_t file_time);
static char get_file_suffix (mode_t mode);
static int long_mode (file_stat_t *stats, size_t n, const char *dir);
static int column_mode (file_stat_t *stats, size_t n, const char *dir);
static int single_mode (file_stat_t *stats, size_t n, const char *dir);
static int get_config (int argc, char **argv, int *p_conf);
static int list_files (char **files, size_t n, char *dir);
static int list_directory (char *dir);
static int list_directories (char **dirs, size_t n, int first);


/* generic */
size_t
filter (void *arr, size_t elem_count, size_t elem_size, int (*cb)(void *cb_data, void *a), void *cb_data)
{
    /* {{{ */
    unsigned char *src = arr;
    unsigned char *dst = arr;
    size_t count = 0;

    if ((arr == NULL) || (!elem_count) || (!elem_size) || (cb == NULL))
    {
        return 0;
    }

    for (; elem_count > 0; elem_count--)
    {
        if (cb (cb_data, src))
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
map (void *arr, size_t elem_count, size_t elem_size, void (*cb)(void *cb_data, void *a), void *cb_data)
{
    /* {{{ */
    unsigned char *iter = arr;

    if ((arr == NULL) || (!elem_count) || (!elem_size) || (cb == NULL))
    {
        return;
    }

    for (; elem_count > 0; elem_count--)
    {
        cb (cb_data, iter);
        iter += elem_size;
    }
    /* }}} */
}

char *
strdup (const char *src)
{
    /* {{{ */
    size_t n = 0;
    char *dst = NULL;

    if (src == NULL)
    {
        return NULL;
    }

    n = strlen (src);
    dst = malloc (n + 1);
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
    
    /* if buf is null, return reqruied */
    if (buf == NULL) return size+1;

    /* otherwise reture # of bytes written (i) */
    if (n == 0) return i;

    n--;
    buf[n] = '\0';
    i++;

    while (i <= n)
    {
        buf[n-i] = '0' + (lu % 10);
        lu /= 10;
        i++;
    }

    return i;
    /* }}} */
}


char *
snprintlu_dup (long unsigned lu)
{
    /* {{{ */
    size_t size = snprintlu (NULL, 0, lu);
    char *buf = malloc (size);
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
    
    if (dir != NULL)
    {
        len = strlen (dir);
        memcpy (iter, dir, len);
        iter += len;
    }

    *iter = '/';
    iter++;

    if (child != NULL)
    {
        len = strlen (child);
        memcpy (iter, child, len);
        iter += len;
    }

    *iter = '\0';

    return s_result; 
    /* }}} */
}

int
dir_content (char **buf, size_t n, const char dirname[])
{
    /* {{{ */
    DIR *dir = NULL; 
    struct dirent *dirent = NULL;
    size_t count = 0;

    if (dirname == NULL) { return -1; }

    dir = opendir (dirname);
    if (dir == NULL)
    {
        perror (_("cannot read directory"));
        return -1;
    }

    /* iterate over the directory contents */
    for (; (dirent = readdir (dir)) != NULL; count++)
    {
        if (buf == NULL) continue;
        if (count + 1 > n) break;
        
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
static int 
sort_alphabetical (const void *a, const void *b)
{
    /* {{{ */
    const file_stat_t *file0 = a;
    const file_stat_t *file1 = b;

    /* TODO: character collation */
    int result = strcoll (file0->filename, file1->filename);

    result *= ((s_conf & SORT_REVERSE) ? -1 : 1);
    return result;
    /* }}} */
}

static int 
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

static int
filter_hidden (void *cb_data, void *a)
{
    /* {{{ */
    char **filename = a;
    int result = (**filename != '.');
    
    UNUSED (cb_data);
    
    /* free non matches */
    if (!result)
    {
        free (*filename);
        *filename = NULL;
    }

    return result;
    /* }}} */
}

static int
filter_special_directories (void *cb_data, void *a)
{
    /* {{{ */
    file_stat_t *stat = a;
    int result0 = (strcmp (stat->filename, "..") != 0);
    int result1 = (strcmp (stat->filename, ".") != 0);

    UNUSED (cb_data);

    return result0 && result1;
    /* }}} */
}

static int
filter_non_directories (void *cb_data, void *a)
{
    /* {{{ */
    file_stat_t *stat = a;
    int result = S_ISDIR (stat->stat.st_mode);
    UNUSED (cb_data);
    return result;
    /* }}} */
}

static void
map_free_str_array (void *cb_data, void *a)
{
    /* {{{ */
    UNUSED (cb_data);
    free (*(char **)a);
    *(char **)a = NULL;
    /* }}} */
}

static void 
map_free_long_fmt (void *cb_data, void *a)
{
    /* {{{ */
    long_fmt_t *fmt = a;
    UNUSED (cb_data);
    free (fmt->date); fmt->date = NULL;
    free (fmt->group); fmt->group = NULL;
    free (fmt->mode); fmt->mode = NULL;
    free (fmt->owner); fmt->owner = NULL;
    /* }}} */
}

static void
map_add_path (void *cb_data, void *a)
{
    /* {{{ */
    char *dir = cb_data;
    file_stat_t *stat = a;
    char *new_filename = strdup (add_child (dir, stat->filename));
    UNUSED (cb_data);
    stat->filename = new_filename;
    /* }}} */
}

static void 
map_print_str_array (void *cb_data, void *a)
{
    /* {{{ */
    file_stat_t *stat = a;
    char *dir = cb_data;

    printf ("%s: %s\n", dir, stat->filename);
    /* }}} */
}

static void
map_free_file_stat (void *cb_data, void *a)
{
    /* {{{ */
    file_stat_t *stat = a;
    UNUSED (cb_data);
    free (stat->filename);
    stat->filename = NULL;
    /* }}} */
}

static void
map_make_printable (void *cb_data, void *a)
{
    /* {{{ */
    char *c = a;
    UNUSED (cb_data);
    if ((!isprint (*c)) || (*c == '\t')) *c = '?';
    /* }}} */
}


/* ls exclusive */
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

    return strdup (buf);
    /* }}} */
}

static char *
get_printable_filename (char *src, mode_t mode)
{
    /* {{{ */
    size_t n = 0;
    char *dst = NULL;
    
    n = strlen (src);
    if (s_conf & SUFFIXES)
    {
        n++;
    }

    dst = malloc (n + 1);
    (void)strcpy (dst, src);

    if (s_conf & SUFFIXES)
    {
        dst[n-1] = get_file_suffix (mode);
    }

    dst[n] = '\0';

    if (!(s_conf & PRINTABLE)) return dst;
    map (dst, strlen (dst), sizeof (char), map_make_printable, NULL);

    return dst;
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

    return strdup (s_buf);
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
        printf (_("total %lu\n"), total_blocks);
    }

    for (i = 0, iter = stats; i < n; i++, iter++)
    {
        if (s_conf & INODE_MODE)
        {
            printf ("%*lu ", max_widths[0], iter->stat.st_ino);
        }

        printf ("%s %*lu %-*s %-*s %*lu %s %s\n",
                fmt[i].mode, 
                max_widths[1], iter->stat.st_nlink, 
                max_widths[2], fmt[i].owner, 
                max_widths[3], fmt[i].group, 
                max_widths[4], iter->stat.st_size, 
                fmt[i].date, 
                iter->printable);
    }

    map (fmt, n, sizeof (*fmt), map_free_long_fmt, NULL);
    free (fmt); fmt = NULL;
    return 0;
    /* }}} */
}

static int 
column_mode (file_stat_t *stats, size_t n, const char *dir)
{
    /* {{{ */
    size_t i = 0;
    size_t j = 0;
    size_t index = 0;
    file_stat_t *iter = NULL;
    int column_width = 0;
    int inode_width = 0;
    int file_width = 0;
    int tmp_width = 0;

    /* const char *seperator = " : "; */
    const char *seperator = "  ";
    int terminal_width = 0;
    int rows = 0;
    int columns = 0;

    const char *env_columns = getenv ("COLUMNS");
    if (env_columns != NULL)
    {
        terminal_width = atoi (env_columns);
    }
    else 
    {
        terminal_width = 80;
    }

    UNUSED (dir);

    for (i = 0, iter = stats; i < n; i++, iter++)
    {
        file_width  = MAX (file_width,  (int)strlen (stats[i].printable));

        tmp_width = 0;
        tmp_width += file_width;
        
        if (s_conf & INODE_MODE)
        {
            inode_width = MAX (inode_width, (int)lu_len (stats[i].stat.st_ino));
            tmp_width += inode_width;
        }

        column_width = MAX (column_width, (int)tmp_width);
    }
    

    columns = terminal_width / (column_width + (int)strlen (seperator));
    rows = (int)ceil ((double)n / columns);
    
    if (columns <= 0)
    {
        return single_mode (stats, n, dir);
    }


    for (i = 0; (int)i < rows; i++)
    {
        for (j = 0; (int)j < columns; j++)
        {
            index = i + j * (size_t)rows;
            if (index >= n) continue;

            if (s_conf & INODE_MODE)
            {
                printf ("%*lu ", inode_width, stats[index].stat.st_ino);
            }

            printf ("%-*s", file_width, stats[index].printable);
            printf ("%s", seperator);
        }

        printf ("\n");
    }

    return -1;
    /* }}} */
}

static int 
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

        printf ("%s\n", iter->printable);
                
    }

    return 0;
    /* }}} */
}

static int
get_config (int argc, char **argv, int *p_conf)
{ 
    /* {{{ */
    int config = SINGLE_MODE | FILE_MOD | SORT_COLLATE;
    int c = 0;

    /* default settings for terminal device */
    if (isatty (1))
    {
        DISABLE_BFLAG (config, PRINT_MODES);
        ENABLE_BFLAG (config, COLUMN_MODE);
        ENABLE_BFLAG (config, PRINTABLE);
    }

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

        case 'q':
            ENABLE_BFLAG (config, PRINTABLE);
            break;

        case 'r':
            ENABLE_BFLAG (config, SORT_REVERSE);
            break;

        case 't':
            DISABLE_BFLAG (config, SORT_MODES);
            ENABLE_BFLAG (config, SORT_TIME);
            break;

        case 'u': 
            DISABLE_BFLAG (config, TIME_MODES);
            ENABLE_BFLAG (config, FILE_ACCESS); 
            break;

        case '1':
            DISABLE_BFLAG (config, PRINT_MODES);
            ENABLE_BFLAG (config, SINGLE_MODE);
            break;

        case '?':
        default:
            (void)printf (_("usage: ls [-CFRacdilqrtu1][file...]\n"));
            return -1;
        }

    }

    *p_conf = config;
    return optind;
    /* }}} */
}

static int
list_files (char **files, size_t n, char *dir)
{
    /* {{{ */
    size_t i = 0;
    file_stat_t *stats = malloc (n * sizeof (file_stat_t));
    char **recurse_buf = NULL;

    /* get stats on all the files */
    for (i = 0; i < n; i++)
    {
        stats[i].filename = files[i];
        (void)stat (add_child (dir, files[i]), &stats[i].stat);
        stats[i].printable = get_printable_filename (stats[i].filename, stats[i].stat.st_mode);

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
    qsort (stats, n, sizeof (*stats), sort_alphabetical);
    if (s_conf & SORT_TIME)
    {
        qsort (stats, n, sizeof (*stats), sort_date);
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
    else if (s_conf & SINGLE_MODE)
    {
        single_mode (stats, n, dir);
    }
    else 
    {
        (void)fprintf (stderr, _("ls: error no mode selected\n"));
        return -1;
    }

    /* recursive */
    if (s_conf & RECURSIVE)
    {
        n = filter (stats, n, sizeof (*stats), filter_non_directories, NULL);
        n = filter (stats, n, sizeof (*stats), filter_special_directories, NULL);
        map (stats, n, sizeof (*stats), map_add_path, dir);

        recurse_buf = malloc (n * sizeof (char *));
        for (i = 0; i < n; i++)
        {
            recurse_buf[i] = stats[i].filename;
        }
        list_directories (recurse_buf, n, 0);

        free (recurse_buf); recurse_buf = NULL;
        map (stats, n, sizeof (*stats), map_free_file_stat, NULL); 
    }

    free (stats); stats = NULL;
    return 0;
    /* }}} */
}

static int
list_directory (char *dir)
{
    /* {{{ */
    int n_entries = 0;
    char **entries = NULL;

    n_entries = dir_content (NULL, 0, dir);
    if (n_entries <= 0)
    {
        return -1;
    }

    entries = malloc (n_entries * sizeof (char *));
    (void)dir_content (entries, n_entries, dir);

    /* remove hidden files */
    if (!(s_conf & HIDDEN))
    {
        n_entries = filter (entries, n_entries, sizeof (*entries), filter_hidden, NULL);
    }

    /* list the contents */
    list_files (entries, n_entries, dir);

    map (entries, n_entries, sizeof (*entries), map_free_str_array, NULL);
    free (entries); entries = NULL;

    return 0;
    /* }}} */
}

static int
list_directories (char **dirs, size_t n, int first)
{
    /* {{{ */
    int rc = 0;
    size_t i = 0;

    for (i = 0; i < n; i++, dirs++)
    {
        if ((n > 1) || (s_conf & RECURSIVE))
        {
            if (!first) printf ("\n");
            printf ("%s:\n", *dirs);
        }

        rc = list_directory (*dirs);
        if (rc != 0)
        {
            return rc;
        }

        first = 0;
    }

    return 0;
    /* }}} */
}

static int
list_unknown (char **args, size_t n)
{
    size_t i = 0;
    struct stat s = { 0 };

    char **files = malloc (sizeof (char *) * n);
    char **dirs  = malloc (sizeof (char *) * n);

    size_t files_n = 0;
    size_t dirs_n = 0;

    for (i = 0; i < n; i++)
    {
        if (stat (args[i], &s)) continue;
        if (S_ISDIR (s.st_mode))
        {
            dirs[dirs_n++] = args[i];
        }
        else
        {
            files[files_n++] = args[i];
        }
    }

    if (files_n != 0)
    {
        list_files (files, files_n, NULL);
    }

    if (dirs_n != 0)
    {
        list_directories (dirs, dirs_n, 1);
    }

    free (dirs);
    free (files);
    return 0;
}

int
ls_main (int argc, char **argv)
{
    /* {{{ */
    int rc = 0;
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
        rc = list_files (argv, argc, "");
    }
    else
    {
        /*rc = list_directories (argv, argc, 1); */
        rc = list_unknown (argv, argc);
    }

    return rc;
    /* }}} */
}

#if 0
int
main (int argc, char **argv)
{
    /* {{{ */
    setlocale (LC_ALL, "");
    setlocale (LC_COLLATE, "");
    setlocale (LC_TIME, "");
    setlocale (LC_CTYPE, "");

    bindtextdomain (DOMAIN_NAME, DOMAIN_DIR);
    textdomain (DOMAIN_NAME);

    return ls_main (argc, argv);
    /* }}} */
}
#endif

/* vim: fdm=marker
 * end of file */
