/* $Id: osgnulnx.h,v 1.3 2012/10/17 01:24:43 ozzmosis Exp $ */

#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#define STR_DIRSEPARATOR "/"

#define MYMAXFILE  64
#define MYMAXDIR   1024
#define MYMAXPATH  1088
#define MYMAXEXT   12
#define MYMAXDRIVE 3

#define filecmp    strcmp
#define filenodir(x) (strchr(x,'/') == NULL)
#define stricmp    strcasecmp

/* Needed definitions */
#define OSFND "osgnufnd.c"
#define OSFUL "oslnxful.c"
/* Overrides over defaults */
#undef OSCWD
#define OSCWD "oslnxcwd.c"
#undef OSDSL
#define OSDSL "oslnxdsl.c"

#define USE_OWN_FGETS "oslnxget.c"

#define HAVE_GETPID

struct _filefind
{
    char path[MYMAXDIR];
    char mask[MYMAXFILE];
    DIR *dirp;
    struct dirent *pentry;
    int flags;
};
