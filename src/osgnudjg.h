/* $Id: osgnudjg.h,v 1.3 2004/07/17 03:05:06 ozzmosis Exp $ */

#include <dir.h>
#include <stdlib.h>             /* for size_t */
#include <io.h>
#include <unistd.h>             /* for chdir, getcwd */
#include <dirent.h>

#define STR_DIRSEPARATOR "\\"

/* Needed definitions */
#define OSFND "osgnufnd.c"
#define OSFUL "osdosful.c"

#define HAVE_STRUPR

#define MYMAXFILE  MAXFILE
#define MYMAXDIR   MAXDIR
#define MYMAXPATH  MAXPATH
#define MYMAXEXT   MAXEXT
#define MYMAXDRIVE MAXDRIVE

struct _filefind
{
    char path[MYMAXDIR];
    char mask[MYMAXFILE];
    DIR *dirp;
    struct dirent *pentry;
    int flags;
};


#define filecmp	   stricmp
#define filenodir(x) (strpbrk((x),"\\/") == NULL)
