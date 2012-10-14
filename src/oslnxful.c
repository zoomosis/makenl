/* $Id: oslnxful.c,v 1.4 2012/10/14 14:49:17 ozzmosis Exp $ */

#define HAVE_OS_FULLPATH
#include <unistd.h>

#include "mklog.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

int os_fullpath(char *dst, const char *src, size_t bufsize)
{
    char olddir[MYMAXDIR];
    char dir[MYMAXDIR];
    char name[MYMAXFILE];
    char ext[MYMAXEXT];

    myfnsplit(src, NULL, dir, name, ext);
    mklog(LOG_DEBUG, "os_fullpath(): dir=%s, name=%s, ext=%s", dir, name, ext);
    getcwd(olddir, sizeof olddir);
    mklog(LOG_DEBUG, "os_fullpath(): old directory=%s", olddir);
    if (dir[0] && chdir(dir) == -1)
    {
        mklog(LOG_ERROR, "os_fullpath(): change directory to '%s' failed!", dir);
        return -1;
    }
    if (getcwd(dir, MYMAXDIR) == NULL || strlen(dir) + strlen(name) + strlen(ext) > bufsize)
    {
        mklog(LOG_ERROR, "os_fullpath(): Directory name for '%s' too long!", src);
        chdir(olddir);
        return -1;
    }
    myfnmerge(dst, NULL, dir, name, ext);
    mklog(LOG_DEBUG, "os_fullpath(): complete filename: %s", dst);
    chdir(olddir);
    return 0;
}
