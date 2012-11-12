/* $Id: ostbcdos.h,v 1.4 2012/11/12 18:04:01 ozzmosis Exp $ */

#include <dir.h>
#include <stdlib.h>             /* for size_t */
#include <io.h>
#include <process.h>

#define STR_DIRSEPARATOR "\\"

/* Needed definitions */
#define OSFND "osdosfnd.c"
#define OSFUL "osdosful.c"

#ifdef USE_DO_EXEC
/* Thomas Wagner's do_exec() isn't working yet... */
/* Overrides over defaults */
#undef OSEXC
#define OSEXC "osd16exc.c"
#endif

#define HAVE_STRUPR
#define HAVE_GETPID

#define __DOS16__
#define F_OK 0

#define MYMAXFILE  MAXFILE
#define MYMAXDIR   MAXDIR
#define MYMAXPATH  MAXPATH
#define MYMAXEXT   MAXEXT
#define MYMAXDRIVE MAXDRIVE

struct _filefind
{
    char path[MYMAXFILE + MYMAXEXT];
    struct ffblk fileinfo;
};

#define filecmp  stricmp
#define filenodir(x) (strpbrk((x),"\\/") == NULL)
#define strcasecmp stricmp

/* vsnprintf() unavailable on very old version of Turbo C, so use insecure vsprintf() */
#define vsnprintf(str, n, fmt, ap) vsprintf(str, fmt, ap)
