/* $Id: osgnuemx.h,v 1.4 2012/11/13 20:02:17 ozzmosis Exp $ */

#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#define STR_DIRSEPARATOR "/"

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE
#define HAVE_OS_FILE_GETNAME
#define HAVE_OS_GETCWD
#define HAVE_OS_FGETS
#define HAVE_STRUPR
#define HAVE_GETPID

#define filecmp  stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

/* Needed definitions */
#define OSFND "osgnufnd.c"
#define OSFUL "osemxful.c"
/* Overrides over defaults */
#undef OSEXC
#define OSEXC "osemxexc.c"
#undef OSGTN
#define OSGTN "ignore.h"
#define os_file_getname(path) _getname(path)

#define os_getcwd _getcwd2

struct _filefind
{
    char path[MYMAXDIR];
    char mask[MYMAXFILE];
    DIR *dirp;
    struct dirent *pentry;
    int flags;
};