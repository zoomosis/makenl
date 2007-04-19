/* $Id: ostbcdos.h,v 1.8 2004-07-17 10:20:21 ozzmosis Exp $ */

#include <dir.h>
#include <stdlib.h>             /* for size_t */
#include <io.h>

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

#define filecmp	   stricmp
#define filenodir(x) (strpbrk((x),"\\/") == NULL)
