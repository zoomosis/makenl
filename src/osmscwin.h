/* $Id: osmscwin.h,v 1.2 2004-07-18 14:48:39 ozzmosis Exp $ */

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <direct.h>

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
