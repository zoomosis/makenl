/* $Id: osmscwin.h,v 1.3 2012/10/17 01:24:43 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>
#include <process.h>

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

#if (_MSC_VER <= 900)
/* vsnprintf() unavailable on very old version of MSVC, so use insecure vsprintf() */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)
#endif
