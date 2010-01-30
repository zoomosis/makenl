/* $Id: oswatxxx.h,v 1.2 2010/01/30 12:22:44 jame Exp $ */

#include <stdlib.h>
#include <dos.h>
#include <direct.h>
#include <io.h>

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
#define __W32__
#endif

#define HAVE_OS_FULLPATH
#define HAVE_OS_FGETS
#define HAVE_STRUPR

#define MYMAXFILE  _MAX_FNAME
#define MYMAXDIR   _MAX_DIR
#define MYMAXPATH  _MAX_PATH
#define MYMAXEXT   _MAX_EXT
#define MYMAXDRIVE _MAX_DRIVE


struct _filefind
{
#if defined(__OS2V2__)
    char path[512];
#elif
    char path[MYMAXFILE + MYMAXEXT];
#endif
    struct find_t fileinfo;
};

#define filecmp	   stricmp
#define filenodir(x) (strpbrk(x,"\\/") == NULL)
