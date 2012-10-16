/* $Id: osborwin.h,v 1.3 2012/10/16 18:15:07 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>

#ifdef __WIN32__
#define __W32__ 1
#endif

#define STR_DIRSEPARATOR "\\"

#define MYMAXFILE  FILENAME_MAX
#define MYMAXDIR   MYMAXFILE
#define MYMAXPATH  MYMAXFILE
#define MYMAXEXT   13
#define MYMAXDRIVE 3
#define filecmp    stricmp
#define filenodir(x) (strchr(x,'/') == NULL)
#define strcasecmp stricmp

/* Needed definitions */
#define OSFND "osmscfnd.c"
#define OSFUL "osmscful.c"
/* Overrides over defaults */
#undef OSCWD
#define OSCWD "osmsccwd.c"

#define HAVE_STRUPR

struct _filefind
{
    char path[MYMAXDIR];
    int handle;
    struct _finddata_t fileinfo;
};
