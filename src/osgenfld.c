/* $Id: osgenfld.c,v 1.2 2004/07/11 09:29:14 ozzmosis Exp $ */

#include <sys/types.h>
#include <sys/stat.h>

/* make an absolute path from given relative path */
/* src is supposed to specify an existing directory name    */
#define FUNCNAME "[generic] os_fulldir"
int os_fulldir(char *dst, const char *src, size_t bufsiz)
{
    char tmp[MYMAXPATH];
    struct stat st;

    Debug1("called with src='%s'\n", src);
    strcpy(tmp, src);
    os_remove_slash(tmp);
    Debug1("after removing slash: '%s'\n", tmp);
    if (os_fullpath(dst, tmp, bufsiz) != 0)
    {
        Debug("os_fullpath failed!\n");
        return -1;
    }
    Debug1("doing stat on %s\n", dst);
    if (stat(dst, &st) != 0)
    {
        Debug("stat failed!\n");
        return -1;
    }
    Debug1("st_mode is now %o\n", st.st_mode);
    return ((st.st_mode & S_IFMT) == S_IFDIR) ? 0 : -1;
}
