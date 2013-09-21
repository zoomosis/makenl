/* $Id: oswatxxx.h,v 1.11 2013/09/21 08:58:56 ozzmosis Exp $ */

#include <stdlib.h>
#include <dos.h>
#include <direct.h>
#include <io.h>
#include <process.h>

#define STR_DIRSEPARATOR "\\"

/* Needed definitions */
#define OSFND "oswatfnd.c"
#define OSFUL "oswatful.c"

#ifdef USE_DO_EXEC
/* Thomas Wagner's do_exec() isn't working yet... */
/* Overrides over defaults */
#undef OSEXC
#define OSEXC "osd16exc.c"
#endif

#ifdef __DOS__
#define __MSDOS__
#ifdef __386__
#define __DOS32__
#else
#define __DOS16__
#endif
#endif

#ifdef __NT__
#ifndef WIN32
#define WIN32 1
#endif
#endif

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
    char path[MYMAXFILE + MYMAXEXT];
    struct find_t fileinfo;
};

#define filecmp stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
#define strcasecmp stricmp

#if __WATCOMC__ <= 1100
/* vsnprintf() unavailable in early versions of Watcom C */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)
#endif
