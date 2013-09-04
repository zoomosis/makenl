/* $Id: osbcos2.h,v 1.3 2013/09/04 00:58:31 ozzmosis Exp $ */

#include <stdlib.h>
#include <dos.h>
#include <direct.h>
#include <dirent.h>
#include <io.h>
#include <process.h>

#define STR_DIRSEPARATOR "\\"

/* Needed definitions */
#define OSFND "oswatfnd.c"
#define OSFUL "oswatful.c"

#define HAVE_OS_FULLPATH
#define HAVE_OS_FGETS
#define HAVE_STRUPR
#define HAVE_GETPID

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE

struct _filefind
{
    char path[512];
    struct find_t fileinfo;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)
