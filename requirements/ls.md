
# POSIX.2-1992 ls

## Flags

    ls [-CFRacdilqrtu1][file...]
    
    -C  write multiple columns, adapt to output device.
    -F  suffix directory with `/`, executable with `*`, and FIFO with a `|`.
    -R  recursively list subdirectories
    -a  write hidden files aswell `.` prefiexed files
    -c  use time file status information's last modified, NOT the file itselfs
    -d  treat directory input same as file input (dont show contents)
    -i  write file seriel number.
    -l  write long format (assume -1)
    -q  replace all non-printable and tab character with `?`
    -r  reverse sort order
    -t  sort by time modified (most recent first)
    -u  use time of last access for sorting and writing
    -1  force output to 1 entry per line

## Environment

    COLUMNS      terminal width for column calculations in `-c`
    LANG         default locale to use
    LC_ALL       override LANG and all LC_* envvars, use single locale for all
    LC_COLLATE   locale specified character collation for pathname sequence
    LC_CTYPE     locale specified single/multibyte character argument parsing
    LC_MESSAGES  message language 
    LC_TIME      locale for date/time
    TZ           timezone to use

## stdout 

default output mode is `-1` (numeric one), except terminals, their output mode 
is implementation defined.

`-i` option use format:
    "%u ", <file serial number>

`-l` option us format:
    "%s %u %s %s %u %s %s\n", <file mode>, <number of links>,
    <owner name>, <group name>, <number of bytes in the file>,
    <date and time>, <path name>

if `<owner name>` or `<group name>` cannot be found, use numeric id's instead.

`<date and time>` field follows locale, 
The POSIX locale should use format 1 for files less than 6 months old,
where as format 2 should be used for files older than 6 months, or where the 
datetime is set into the future.

  1. date "+%b %e %H:%M"
  2. date "+%b %e  %Y"

file size is implementation defined for character and block special files.

if a pathname was specified as a file operand, write it as specified, no 
substitions or reductions.

`<file mode>` shall follow format:
    "%c%s%s%s%c", <entry type>, <owner permissions>,
    <group permissions>, <other permissions>, 
    <optional alternate access method flag>

`<optional alternate access method flag>` use space character if not applicable.

`<entry type>` describes the type of file, where:
    d  directory
    b  block special file
    c  character special file
    p  FIFO
    -  regular file

implementation defined types may be added.

`<.* permissions>` feilds should follow the following format, where:
  1. `r` if readable, `-` otherwise
  2. `w` if readable, `-` otherwise
  3. the first of the following applies
    a. `S` if set-user-ID is set, and the owner does not have execute perms
    b. `s` if set-user-ID is set, and the owner does have execute perms
    c. `x` if the file is executble or the directory is searchable.
    d. `-` if none of the above apply.
    
    implementations may add other characters to this field, such that all 
    lowercase indicate the file is executable or searchable, and uppercase if 
    not.

`-l` each list of files should be preceeded by a sttus line indecating the 
number of file system blocks occupied by files in 512B units, rounded up. 
for example, in the posix locale:
    "total %u\n", <number of units in the directory>

if more than one directory, or a combination of nondirectoyr files and 
directrories are written, (either by multiple operands, or the `-R` options) 
each list of files should be preceeded by:
    "\n%s:\n", <directory name>

if this string is the first item to b writtne, the first `<newline>` character
should be skipped. (this preceeds the number of units in the directory).

## stderr

error messages only

## exit status

     0  all files written successfully,
    >0  an error occured.
