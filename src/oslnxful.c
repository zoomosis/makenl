/* $Id: oslnxful.c,v 1.4 2004/07/15 17:44:02 ozzmosis Exp $ */

#define HAVE_OS_FULLPATH
#include <unistd.h>

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
#define FUNCNAME ""
    Debug1("[linux] os_fullpath: dir=%s", dir);
    Debug1(" name=%s", name);
    Debug1(" ext=%s\n", ext);
    getcwd(olddir, sizeof olddir);
#undef FUNCNAME
#define FUNCNAME "[linux] os_fullpath"
    Debug1("old directory=%s\n", olddir);
    if (dir[0] && chdir(dir) == -1)
    {
        Debug1("change directory to '%s' failed!\n", dir);
        return -1;
    }
    if (getcwd(dir, MYMAXDIR) == NULL
        || strlen(dir) + strlen(name) + strlen(ext) > bufsize)
    {
        fprintf(stderr, "Directory name for %s too long!\n", src);
        chdir(olddir);
        return -1;
    }
    myfnmerge(dst, NULL, dir, name, ext);
    Debug1("Complete filename: %s\n", dst);
    chdir(olddir);
    return 0;
#undef FUNCNAME
}
