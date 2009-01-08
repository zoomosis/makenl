/* $Id: osborwin.h,v 1.1 2009/01/08 20:07:47 mbroek Exp $ */

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
#define filecmp	   stricmp
#define filenodir(x) (strchr(x,'/') == NULL)

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
