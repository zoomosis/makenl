/* $Id: osborwin.h,v 1.5 2012/11/12 16:33:15 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <process.h>

#ifdef __WIN32__
#ifndef WIN32
#define WIN32 1
#endif
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
#define HAVE_GETPID

struct _filefind
{
    char path[MYMAXDIR];
    int handle;
    struct _finddata_t fileinfo;
};
